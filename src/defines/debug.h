#ifndef DEBUG_H
#define DEBUG_H
#include <iostream>
#ifdef CMS_DEBUG
  #include <cppcms/http_context.h>
  #include <cppcms/http_request.h>
  #include "extentions.h"

  #define CMS_DEBUG_LOG(_msg) std::cout << _msg << std::endl
  #define CMS_DEBUG_CONTEXT void main(std::string url) { CMS_DEBUG_LOG("Path: " << request().path_info()); }

  #define DF_DO_SQL_DEBUG
  #ifdef DF_DO_SQL_DEBUG
    class CDebugPrintStream
    {
      public:
        CDebugPrintStream()  { std::cout << "Values: "; }
        ~CDebugPrintStream() { std::cout << std::endl ; }
        CDebugPrintStream &go()                                 {                     return *this; }
        CDebugPrintStream &operator()(const int        & value) { std::cout << "(" << value << ")"; return *this; }
        CDebugPrintStream &operator()(const std::string& value) { std::cout << "(" << value << ")"; return *this; }
    };
    #define CMS_DEBUG_LOG_STREAM(_values) \
      CDebugPrintStream *ps = new CDebugPrintStream(); \
      ps->go() _values; \
      delete ps;
    #define DF_SQL_DEBUG(_msg) CMS_DEBUG_LOG(_msg)
  #else
    #define CMS_DEBUG_LOG_STREAM(_values)
    #define DF_SQL_DEBUG(_msg)
  #endif

  //#define DF_DO_RIGHTS_DEBUG
  #ifdef DF_DO_RIGHTS_DEBUG
    #define DF_RIGHTS_DEBUG(_msg) CMS_DEBUG_LOG(_msg)
  #else
     #define DF_RIGHTS_DEBUG(_msg)
  #endif

#else
  #define CMS_DEBUG_LOG(_msg)
  #define CMS_DEBUG_CONTEXT
  #define CMS_DEBUG_POST
  #define CMS_DEBUG_LOG_STREAM(_values)
  #define DF_SQL_DEBUG(_msg)
  #define DF_RIGHTS_DEBUG(_msg)
#endif

#endif // DEBUG_H
