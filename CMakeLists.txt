# %Id%
include(CMakeCompiler.cmake)
PROJECT(docflow)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

include(CMakeDebug.cmake)

OPTION ( PREPROCESS "Preprocess html in project" OFF )

ADD_DEFINITIONS( -DMAGICKCORE_QUANTUM_DEPTH=16 )
ADD_DEFINITIONS( -DMAGICKCORE_HDRI_ENABLE=1 )
IF( PREPROCESS )
    ADD_DEFINITIONS( -DCMS_PREPROCESS )
ENDIF( PREPROCESS )

set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_LIBS ON)
set(BOOST_COMPONENTS
        system
        filesystem
        regex)

find_package(ImageMagick COMPONENTS Magick++)
find_package(Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
find_package(Threads)

set( GEN_PATH "${PROJECT_BINARY_DIR}/cms_generated_sources" )
file(MAKE_DIRECTORY ${GEN_PATH})
set(PREPROCESSED_FILES)

message("Compiling less css...")
set(GEN_CSS_FILE ${GEN_PATH}/style.css)
execute_process(COMMAND lessc ${lessc_extra_keys} --include-path=${CMAKE_SOURCE_DIR}/src/media/less/  ${CMAKE_SOURCE_DIR}/src/media/less/style.less ${GEN_CSS_FILE})

message("Closuring js code...")
set(GEN_JS_FILE ${GEN_PATH}/script.js)
FILE(STRINGS "${CMAKE_SOURCE_DIR}/src/media/js/scripts-to-include.txt" js_files)
execute_process(COMMAND closure-compiler ${closure_extra_keys} --summary_detail_level 3 --compilation_level=SIMPLE_OPTIMIZATIONS --js_output_file ${GEN_JS_FILE} ${js_files}
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/media/js/")

macro(preprocess_html file)
    string(REPLACE "/" "_" tmp_var "${file}")
    set(out_file ${GEN_PATH}/${tmp_var}.preprocessed.cpp)
    list(APPEND PREPROCESSED_FILES "${out_file}")
    execute_process(COMMAND ${CMAKE_SOURCE_DIR}/tools/preprocessor.pl ${CMAKE_SOURCE_DIR}/${file} ${out_file} ${CMAKE_SOURCE_DIR}/tools/preprocessor_templates)
endmacro(preprocess_html)


include_directories(${Boost_INCLUDE_DIRS}
                    ${ImageMagick_INCLUDE_DIRS}
                    ${SOAP_GEN_PATH}
                    src
                    src/db
                    src/defines
                    src/rights
                    src/model
                    src/model/auth)

SET(workflow-headers
    src/capplication.h
    src/db/cdatabase.h
    src/db/cquery.h
    src/db/cconnection.h
    src/extentions.h
    src/clogger.h
    src/cmailer.h
    src/model/chtmlgenerator.h
    src/model/cpagepart.h
    src/model/cpage.h
    src/model/cauthorize.h
    src/model/cuserpage.h
    src/model/cmainpage.h
    src/model/cmenu.h
    src/model/ctasks.h
    src/model/cmembers.h
    src/model/cdivisions.h
    src/model/chelp.h
    src/model/cimage.h
    src/model/ccalendar.h
    src/model/cattachedfiles.h
    src/model/cmaintenancework.h
    src/model/cmaintenanceworkclass.h
    src/rights/crights.h
    src/defines/html.h
    src/defines/global.h
    src/defines/debug.h
    src/fileutils.h)
SET(workflow-sources
    src/capplication.cpp
    src/db/cdatabase.cpp
    src/db/cquery.cpp
    src/db/cconnection.cpp
    src/extentions.cpp
    src/clogger.cpp
    src/cmailer.cpp
    src/model/cpagepart.cpp
    src/model/cimage.cpp
    src/rights/crights.cpp
    src/fileutils.cpp
    src/main.cpp)
preprocess_html(src/model/cpage.cpp)
preprocess_html(src/model/cauthorize.cpp)
preprocess_html(src/model/cuserpage.cpp)
preprocess_html(src/model/cmainpage.cpp)
preprocess_html(src/model/cmenu.cpp)
preprocess_html(src/model/ctasks.cpp)
preprocess_html(src/model/cmembers.cpp)
preprocess_html(src/model/chtmlgenerator.cpp)
preprocess_html(src/model/cdivisions.cpp)
preprocess_html(src/model/chelp.cpp)
preprocess_html(src/model/cattachedfiles.cpp)
preprocess_html(src/model/ccalendar.cpp)
preprocess_html(src/model/cmaintenancework.cpp)
preprocess_html(src/model/cmaintenanceworkclass.cpp)
#preprocess_html()
#preprocess_html()
#preprocess_html()
#preprocess_html()
#preprocess_html()
add_executable(workflow
               ${workflow-headers}
               ${workflow-sources}
               ${PREPROCESSED_FILES})
target_link_libraries(workflow cppcms pqxx booster ldap PocoNet PocoNetSSL PocoFoundation ${CMAKE_THREAD_LIBS_INIT} ${ImageMagick_LIBRARIES} ${Boost_LIBRARIES} )

install(TARGETS workflow RUNTIME DESTINATION . )
IF( BUILD_DEBUG )
  install(FILES config/workflow-debug.conf DESTINATION . )
else( BUILD_DEBUG )
  install(FILES config/workflow.conf DESTINATION . )
endif( BUILD_DEBUG )
install(DIRECTORY media DESTINATION . )
install(FILES ${GEN_CSS_FILE} DESTINATION ./media/css )
install(FILES ${GEN_JS_FILE} DESTINATION ./media/js )
install(DIRECTORY data DESTINATION . )


