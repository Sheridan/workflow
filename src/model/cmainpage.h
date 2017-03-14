#ifndef CMAINPAGE_H
#define CMAINPAGE_H
#include "cuserpage.h"

namespace df
{
namespace ui
{

class CMainPage : public CUserPage
{
  public:
    CMainPage(cppcms::service &srv);
    ~CMainPage();
  protected:
    void page();

  private:
    void composePageHead();

};

}
}
#endif // CMAINPAGE_H
