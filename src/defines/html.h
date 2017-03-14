#ifndef HTML_H
#define HTML_H
#include <cppcms/form.h>

#define ECHO response().out() <<

#ifdef CMS_PREPROCESS
  #define HTML(...) ECHO __VA_ARGS__
#else
  #define HTML(...) ECHO #__VA_ARGS__
#endif

#endif // HTML_H
