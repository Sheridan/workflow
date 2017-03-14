#ifndef CMAINTENANCEWORKCLASS_H
#define CMAINTENANCEWORKCLASS_H
#include "cuserpage.h"

namespace df
{
namespace ui
{

class CMaintenanceWorkClass : public CUserPage
{
  public:
    CMaintenanceWorkClass(cppcms::service &srv);
    ~CMaintenanceWorkClass();
  private:
    void composePageHead();
    void defaultPage();
    void classesList();
    void addClass();
};

}
}
#endif // CMAINTENANCEWORKCLASS_H
