#include "cpage.h"
#include "html.h"

namespace df
{
namespace ui
{

CPage::CPage(cppcms::service &srv)
  : CHTMLGenerator(srv)
{}

CPage::~CPage()
{}

void CPage::putBaseHeader(const bool &redirectIfNoAuth)
{
  if(!userIsLoggedOn() && redirectIfNoAuth) { response().set_redirect_header("/auth", cppcms::http::response::see_other); }
  m_canRender = composeHTMLHeaders();
  if(m_canRender)
  {
    HTML
    (
      <!++
      <!DOCTYPE html>
      <html>
      <head>
        <meta http-equiv='Content-Type' content='text/html; charset=utf-8'>
        <meta charset='utf-8'>
        <link rel='stylesheet' type='text/css' href='/media/css/jquery-ui/jquery-ui.css' />
        <link rel='stylesheet' type='text/css' href='/media/css/jquery-ui/jquery.datetimepicker.css' />
        <link rel='stylesheet' type='text/css' href='/media/css/pupload/css/jquery.ui.plupload.css' />
        <link rel='stylesheet' type='text/css' href='/media/css/style.css' />
        <script src='/media/js/jquery.min.js' type='text/javascript'></script>
        <script src='/media/js/jquery-ui.min.js' type='text/javascript'></script>
        <script src='/media/js/jquery.ui.datepicker-ru.js' type='text/javascript'></script>
        <script src='/media/js/jquery.datetimepicker.js' type='text/javascript'></script>
        <script src='/media/js/jquery.cookie.js' type='text/javascript'></script>
        <script src='/media/js/pupload/plupload.full.min.js' type='text/javascript'></script>
        <script src='/media/js/pupload/jquery.ui.plupload.min.js' type='text/javascript'></script>
        <script src='/media/js/pupload/pupload.ru.js' type='text/javascript'></script>
        <script src='/media/js/script.js' type='text/javascript'></script>
        <f++ composeHead(); ++f>
      </head>
      <body>
        <div id='dialog_help' class='dialog' title='Помощь'></div>
        <header><f++ composePageHeader(); ++f></header>
        <main>
      ++!>
    );
  }
}

void CPage::putBaseFooter()
{
  if(m_canRender)
  {
    HTML
    (
      <!++
      </main>
      <footer>
          <f++ composePageFooter(); ++f>
      </footer>
      </body>
      </html>
      ++!>
    );
  }
}

}
}
