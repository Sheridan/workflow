#ifndef CMENU_H
#define CMENU_H
#include "cpagepart.h"

namespace df
{
namespace ui
{

class CMenu : public CPagePart
{
  public:
    CMenu(cppcms::service &srv);
    ~CMenu();

  protected:
    void menu();
};

}
}
#endif // CMENU_H
