#ifndef CAUTHORIZE_H
#define CAUTHORIZE_H
#include "cpage.h"

namespace df
{
namespace ui
{

class CAuthorize : public CPage
{
  public:
    CAuthorize(cppcms::service &srv);
    ~CAuthorize();
  protected:
    void authorize();
    void quit();
  private:
    virtual bool composeHTMLHeaders() final;
    virtual void composeHead       () final;
    virtual void composePageHeader () final;
    virtual void composePageFooter () final;
};

}
}
#endif // CAUTHORIZE_H
