#ifndef CMAINTENANCEWORK_H
#define CMAINTENANCEWORK_H
#include "cuserpage.h"

namespace df
{
namespace ui
{

class CMaintenanceWork : public CUserPage
{
  public:
    CMaintenanceWork(cppcms::service &srv);
    ~CMaintenanceWork();
  private:
    void composePageHead();
    void defaultPage();
};

}
}
#endif // CMAINTENANCEWORK_H
