#ifndef CUSERPAGE_H
#define CUSERPAGE_H
#include "cpage.h"
namespace df
{
namespace ui
{

class CUserPage : public CPage
{
  public:
    CUserPage(cppcms::service &srv);
    ~CUserPage();

  protected:
    virtual bool composeHTMLHeaders() final;
    virtual void composeHead() final;
    virtual void composePageHead() = 0;
    virtual void composePageHeader() final;
    virtual void composePageFooter() final;
};

}
}
#endif // CUSERPAGE_H
