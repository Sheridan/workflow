#ifndef CAPPLICATION_H
#define CAPPLICATION_H
#include <cppcms/application.h>
#include <cppcms/service.h>
#include "debug.h"

namespace df
{
namespace model
{

class CApplication : public cppcms::application
{
  public:
    CApplication(cppcms::service &srv);
    ~CApplication();
//    CMS_DEBUG_CONTEXT;
};

}
}
#endif // CAPPLICATION_H
