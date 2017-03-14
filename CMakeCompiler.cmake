option(USE_CLANG "build application with clang" OFF)

if( USE_CLANG )
  MESSAGE ( STATUS "---> Build with clang" )
  set( CMAKE_C_COMPILER clang)
  set( CMAKE_CXX_COMPILER clang++)
  set(USE_GCC OFF)
else( USE_CLANG )
  MESSAGE ( STATUS "---> Build with gcc" )
  set(CMAKE_C_COMPILER gcc)
  set(CMAKE_CXX_COMPILER g++)
endif( USE_CLANG )

ADD_DEFINITIONS( -std=c++11 )
