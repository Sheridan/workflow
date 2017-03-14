#include "cmainpage.h"
#include "html.h"
#include "global.h"
#include "cdatabase.h"
#include <regex>

namespace df
{
namespace ui
{

CMainPage::CMainPage(cppcms::service &srv)
  : CUserPage(srv)
{
  dispatcher().assign(""        , &CMainPage::page           , this);
}

CMainPage::~CMainPage()
{}

void CMainPage::composePageHead()
{
  HTML
  (
    <!++
    <title>Документооборот</title>
    ++!>
  );
}


void CMainPage::page()
{
  putBaseHeader();
  HTML
  (
    <!++
        <panel++ [Список всех текущих задач всех управляемых мной отделов] [ ] [visible] [tasklist]>
          <paged_list++ [division_task_list_open] [/tasks/list] [taskListRenderer]>
            'status'   : 'open_and_not_assigned',
            'member_id': <v++ DF_SESSION_MEMBER_ID_STR ++v>
          <++paged_list>
        <++panel>
    ++!>
  );
  putBaseFooter();
}

}
}
