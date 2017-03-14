#ifndef CPAGE_H
#define CPAGE_H
#include "chtmlgenerator.h"

namespace df
{
namespace ui
{

class CPage : public CHTMLGenerator
{
  public:
    CPage(cppcms::service &srv);
    virtual ~CPage();

  protected:

    void putBaseHeader(const bool &redirectIfNoAuth = true);
    void putBaseFooter();

    virtual bool composeHTMLHeaders() = 0;
    virtual void composeHead()        = 0;
    virtual void composePageHeader()  = 0;
    virtual void composePageFooter()  = 0;

  private:
    bool m_canRender;
};

#define DF_STOP_IF_NO_AUTH if(!userIsLoggedOn()) { return; }

}
}
#endif // CPAGE_H
