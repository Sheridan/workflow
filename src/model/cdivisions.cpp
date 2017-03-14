#include "cdivisions.h"
#include "cquery.h"
#include "html.h"
#include "crights.h"
//#include "cmailer.h"

namespace df
{
namespace ui
{

CDivisions::CDivisions(cppcms::service &srv)
  : CUserPage(srv)
{
  dispatcher().assign(""           , &CDivisions::defaultPage         , this   );
  dispatcher().assign("list"       , &CDivisions::divisionsList       , this   );
  dispatcher().assign("add"        , &CDivisions::divisionAdd         , this   );
  dispatcher().assign("show/(\\d+)", &CDivisions::showDivision        , this, 1);
  dispatcher().assign("edit/(\\d+)", &CDivisions::makeDivisionControls, this, 1);
}

CDivisions::~CDivisions()
{

}

void CDivisions::composePageHead()
{
  HTML(<!++ <title>Подразделения</title> ++!>);
}

void CDivisions::defaultPage()
{
  putBaseHeader();
  HTML
  (
    <!++
      <panel++ [Список подразделений] [ ] [visible] [divisions_list]>
        <div id='divisionslist'></div>
        <thread_load++ ['/divisions/list'] [divisionslist]>
      <++panel>
    ++!>
  );
  putBaseFooter();
}

void CDivisions::divisionsList()
{
  DF_STOP_IF_NO_AUTH
  DFDB_R divisions = DFDB_Q("divisions_list",
        "select "
          /* 0*/   "divisions.id, "
          /* 1*/   "divisions.name,  "
          /* 2*/   "divisions.short_name,  "
          /* 3*/   "members.id as manager_id, "
          /* 4*/   "members.name as manager_name, "
          /* 5*/   "parent_division.id as parent_division_id, "
          /* 6*/   "parent_division.name as parent_division_name, "
          /* 7*/   "parent_division.short_name as parent_division_short_name, "
          /* 8*/   "(select count(workers.id) from members workers where workers.division_id = divisions.id) as workers, "
          /* 9*/   "(select count(tasks_open.id) from tasks tasks_open where tasks_open.target_division_id=divisions.id and tasks_open.status in ('open', 'not_assigned')) as opened_tasks, "
          /*10*/   "(select count(tasks_closed.id) from tasks tasks_closed where tasks_closed.target_division_id=divisions.id and tasks_closed.status in ('closed')) as closed_tasks "
        "from divisions "
        "left join members on divisions.manager_member_id=members.id "
        "left join divisions parent_division on divisions.parent_division=parent_division.id "
        "where divisions.id>0 "
        "order by divisions.parent_division, divisions.name "
                    , /* empty */);
  HTML
  (
    <!++
      <table class="divisions_list">
      <tr>
        <th>Наименование подразделения</th>
        <th>Руководитель</th>
        <th>Вышестоящее подразделение</th>
        <th>Работников</th>
        <th>Задач на исполнении</th>
        <th>Задач исполнено</th>
      </tr>
    ++!>
  );
  DFDB_QLOOP(divisions, division)
  {
    HTML
    (
      <!++
          <tr>
            <td class='division'>
                <a href='/divisions/show/<v++ division[0].as<std::string>() ++v>'>
                        <v++ division[1].as<std::string>() ++v> (<v++ division[2].as<std::string>() ++v>)
                </a>
            </td>
            <td><a href='/members/show/<v++ division[3].as<std::string>() ++v>'><v++ division[4].as<std::string>() ++v></a></td>
            <td class='division'>
                <a href='/divisions/show/<v++ division[5].as<std::string>() ++v>'>
                        <v++ division[6].as<std::string>() ++v> (<v++ division[7].as<std::string>() ++v>)
                </a>
            </td>
            <td><v++ division[8].as<std::string>() ++v></td>
            <td><v++ division[9].as<std::string>() ++v></td>
            <td><v++ division[10].as<std::string>() ++v></td>
          </tr>
      ++!>
    );
  }
  HTML(<!++ </table> ++!>);
}

void CDivisions::showDivision(std::string id)
{
  putBaseHeader();
  DFDB_R division = DFDB_Q("division_show",
        "select "
          /* 0*/   "divisions.id, "
          /* 1*/   "divisions.name,  "
          /* 2*/   "divisions.short_name,  "
          /* 3*/   "members.id as manager_id, "
          /* 4*/   "members.name as manager_name, "
          /* 5*/   "parent_division.id as parent_division_id, "
          /* 6*/   "parent_division.name as parent_division_name, "
          /* 7*/   "parent_division.short_name as parent_division_short_name "
        "from divisions "
        "left join members on divisions.manager_member_id=members.id "
        "left join divisions parent_division on divisions.parent_division=parent_division.id "
        "where divisions.id=$1 "
        "order by divisions.parent_division, divisions.name "
                    , (id));
  if(division.size())
  {
    HTML
    (
      <!++
        <panel++ [Основное] [ ] [visible] [division_main]>
        <table class='division_show'>
        <tr>
          <td colspan='2'>
            <a href='/divisions/show/<v++ division[0][0].as<std::string>() ++v>'>
                    <v++ division[0][1].as<std::string>() ++v> (<v++ division[0][2].as<std::string>() ++v>)
            </a>
          </td>
        </tr>
        <tr>
          <th>Вышестоящее подразделение<help++ [divisionParentDivision]></th>
          <th>Руководитель<help++ [divisionManager]></th>
        </tr>
        <tr>
          <td>
             <a href='/divisions/show/<v++ division[0][5].as<std::string>() ++v>'>
                     <v++ division[0][6].as<std::string>() ++v> (<v++ division[0][7].as<std::string>() ++v>)
             </a>
          </td>
          <td>
           <a href='/members/show/<v++ division[0][3].as<std::string>() ++v>'>
                   <v++ division[0][4].as<std::string>() ++v>
           </a>
          </td>
        </tr>
        </table>
        <++panel>
        <panel++ [Список задач] [ ] [visible] [tasklist]>
        <table class='tasklist'>
          <tr><th>В работе</th><th>Исполнено</th></tr>
          <tr>
            <td>
              <paged_list++ [division_task_list_open] [/tasks/list] [taskListRenderer]>
                'status'   : 'open_and_not_assigned',
                'division_id': <v++ id ++v>
              <++paged_list>
            </td>
            <td>
              <paged_list++ [division_task_list_closed] [/tasks/list] [taskListRenderer]>
                'status'   : 'closed',
                'division_id': <v++ id ++v>
              <++paged_list>
          </td>
          </tr>
        </table>
        <++panel>
        <f++ makeDivisionControls(id); ++f>
      ++!>
    );
  }
  putBaseFooter();
}

void CDivisions::divisionAdd()
{
  if(DF_REQUEST_IS_POST)
  {
    DF_STOP_IF_NO_AUTH
    DFDB_R newDivision = DFDB_Q("new_division", "insert into divisions ("
                                "name, "
                                "short_name, "
                                "parent_division, "
                                "manager_member_id"
                              ") values ($1, $2, $3, $4) returning id",
                               (postValue("name"))
                               (postValue("short_name"))
                               (postValue("division_id"))
                               (postValue("manager_id")));
    int divisionID = CMS_SQL_LAST_INSERT_ID(newDivision);
    updateChangelog(divisionID, "division_created",
                    "Создано подразделение <a href='/divisions/show/" + std::to_string(divisionID) + "'>" + postValue("name") + "</a>");
    response().set_redirect_header("/divisions/", cppcms::http::response::see_other);
  }
  else
  {
//    DF_MAILER->send("sheridan@sergos.ru", "test", "test");
    putBaseHeader();
    HTML
    (
      <!++
      <panel++ [Добавление подразделения] [ ] [visible] [division_add]>
       <f++ divisionEditForm(<s++ /divisions/add ++s>); ++f>
      <++panel>
      ++!>
    );
    putBaseFooter();

  }
}

void CDivisions::makeDivisionControls(std::string id)
{
  if(CMS_RIGHTS->checkDivision(df::rights::rtCanAdd, DF_SESSION_MEMBER_ID_INT))
  {
    if(DF_REQUEST_IS_POST)
    {
      DF_STOP_IF_NO_AUTH
      DFDB_Q("edit_division",
                    "update divisions set "
                      "name=$1, "
                      "short_name=$2, "
                      "parent_division=$3, "
                      "manager_member_id=$4 "
                    "where id=$5",
                     (postValue("name"))
                     (postValue("short_name"))
                     (postValue("division_id"))
                     (postValue("manager_id"))
                     (id));
      updateChangelog(atoi(id.c_str()), "division_edited", "Подразделение "
                      "<a href='/divisions/show/" + id + "'>" + postValue("name") + "</a> изменено");
      response().set_redirect_header("/divisions/show/" + id, cppcms::http::response::see_other);
    }
    else
    {
      HTML(<!++ <panel++ [Редактирование] [ ] [unvisible] [division_edit]> ++!>);
      DFDB_R division = DFDB_Q("division_show_edit",
            "select "
       /* 0*/ "divisions.id, "
       /* 1*/ "divisions.name, "
       /* 2*/ "divisions.short_name, "
       /* 3*/ "divisions.parent_division, "
       /* 4*/ "divisions.manager_member_id "
            "from divisions "
            "where divisions.id=$1 "
            , (id));
      divisionEditForm("/divisions/edit/" + id,
                     division[0][1].as<std::string>(),
                     division[0][2].as<std::string>(),
                     division[0][3].as<int>(),
                     division[0][4].as<int>() );
      HTML(<!++ <++panel> ++!>);
    }
  }
}

void CDivisions::divisionEditForm(const std::string &action,
                                  const std::string &name,
                                  const std::string &shortName,
                                  const int &parentDivisionID,
                                  const int &managerID)
{
  HTML
  (
    <!++
     <form method='POST' action='<v++ action ++v>'>
       <table class='form'>
       <tr>
        <td>
          <labeled_control++ [Полное наименование] [divisionLongName]>
          <input type="text" id="name" name="name" value='<v++ name ++v>'/>
          <++labeled_control>
        </td>
        <td>
          <labeled_control++ [Краткое наименование] [divisionShortName]>
          <input type="text" id="short_name" name="short_name" value='<v++ shortName ++v>'/>
          <++labeled_control>
        </td>
         <td><f++ divisionsSelectBox(<s++Вышестоящее подразделение++s>, <s++divisionParentDivision++s>, 0, parentDivisionID); ++f></td>
        <td><f++ membersSelectBox(<s++Руководитель++s>, <s++manager++s>, <s++divisionManager++s>, 0, managerID); ++f></td>
       </tr>
       </table>
       <input type="submit" value="Отправить" />
     </form>
    ++!>
  );
}

void CDivisions::updateChangelog(const int &divisionID, const std::string &what, const std::string &description)
{
  CHTMLGenerator::updateChangelog("division", divisionID, what, description);
}

}
}
