#include "cuserpage.h"
#include "cquery.h"
#include "html.h"
#include "debug.h"
#include <regex>

namespace df
{
namespace ui
{

CUserPage::CUserPage(cppcms::service &srv)
  : CPage(srv)
{}

CUserPage::~CUserPage()
{}

bool CUserPage::composeHTMLHeaders()
{
  return true;
}

void CUserPage::composeHead()
{
  HTML
  (
    <!++
    ++!>
  );
  composePageHead();
}

void CUserPage::composePageHeader()
{
  std::string memberName = "";
  if(userIsLoggedOn())
  {
    DFDB_R member = DFDB_Q("member_name_and_last_seen",
                           R"s(select 
                            id, 
                            name, 
                            to_char(last_seen_timestamp, 'DD.MM.YYYY HH24:MI') 
                           from members where id=$1)s", (session()["member_id"]));
    memberName = member[0][1].as<std::string>() + ", последнее посещение: " + member[0][2].as<std::string>();
    DFDB_Q("member_last_seen_update", "update members set last_seen_timestamp=now() where id=$1", (member[0][0].as<int>()));
  }
  HTML
  (
    <!++
        <table class='head'>
          <tr>
            <td class='logo'>
              <a href='/' title='На главную'>
                <img class='logo' src='/media/img/logo/ord.png' alt='Логотип'/>
              </a>
            </td>
            <td class='title'>
              Документооборот. Санаторий Орджоникидзе - филиал ФБГУ "ФМЦ" Росимущества<br />
              <v++ memberName ++v>
            </td>
          </tr>
        </table>
      <nav id='menu'></nav>
      <thread_load++ ['/menu'] [menu]>
    ++!>
  );
}

void CUserPage::composePageFooter()
{
  HTML
  (
    <!++
    <a href="http://task.sergos.ru/">Документооборот</a>
    ++!>
  );
}

}
}
