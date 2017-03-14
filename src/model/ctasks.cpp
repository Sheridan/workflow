#include "ctasks.h"
#include "cquery.h"
#include "html.h"
#include "crights.h"
#include "extentions.h"
#include "cfileuploader.h"
#include <sstream>
#include <cppcms/json.h>
#include <boost/filesystem.hpp>

namespace df
{
namespace ui
{

CTasks::CTasks(cppcms::service &srv)
  : CUserPage(srv)
{
  dispatcher().assign("add"                          , &CTasks::addTask           , this         );
  dispatcher().assign("addcomment/(\\d+)"            , &CTasks::addComment        , this, 1      );
  dispatcher().assign("comments/(\\d+)"              , &CTasks::listComments      , this, 1      );
  dispatcher().assign("show/(\\d+)"                  , &CTasks::showTask          , this, 1      );
  dispatcher().assign("changelog/(\\d+)"             , &CTasks::showChangelog     , this, 1      );
  dispatcher().assign("list"                         , &CTasks::taskList          , this         );
  dispatcher().assign("daytotal/(\\d+)/(\\d+)/(\\d+)", &CTasks::dayTotal          , this, 1, 2, 3);
  dispatcher().assign("edit/(\\d+)"                  , &CTasks::editTask          , this, 1      );
  dispatcher().assign(""                             , &CTasks::defaultPage       , this         );
}

CTasks::~CTasks()
{

}

void CTasks::composePageHead()
{
  HTML(<!++ <title>Задачи</title> ++!>);
}

void CTasks::defaultPage()
{
  putBaseHeader();
  std::string memberID = DF_SESSION_MEMBER_ID_STR;
  HTML
  (
    <!++
      <panel++ [Список контролей] [ ] [visible] [tasklcontrolist]>
      <table class='tasklist'>
        <tr><th>Задачи лично мне<help++ [taskForMe]></th><th>Задачи под моим контролем<help++ [taskMyControl]></th></tr>
        <tr>
          <td>
            <paged_list++ [tasks_my] [/tasks/list] [taskListRenderer]>
              'status': 'open',
              'to_me'  : 1
            <++paged_list>
          </td>
          <td>
            <paged_list++ [tasks_controlled_me] [/tasks/list] [taskListRenderer]>
              'status'    : 'open',
              'my_control': 1
            <++paged_list>
          </td>
        </tr>
        <tr><th>Задачи, в которых я участвую</th><th>Созданные мной задачи</th></tr>
        <tr>
          <td>
            <paged_list++ [i_am_coworker] [/tasks/list] [taskListRenderer]>
              'status': 'open',
              'i_am_coworker'  : 1
            <++paged_list>
          </td>
          <td>
            <paged_list++ [i_am_creator] [/tasks/list] [taskListRenderer]>
              'status'    : 'open',
              'i_am_creator': 1
            <++paged_list>
          </td>
        </tr>
      </table>
      <++panel>
      <panel++ [Список личных задач и задач управляемых отделов] [ ] [visible] [tasklist]>
      <table class='tasklist'>
        <tr><th>В работе<help++ [taskInWork]></th><th>Выполнено<help++ [taskFinished]></th></tr>
        <tr>
          <td>
            <paged_list++ [tasks_open_and_not_assigned] [/tasks/list] [taskListRenderer]>
              'status'   : 'open_and_not_assigned',
              'member_id': <v++ memberID ++v>
            <++paged_list>
          </td>
          <td>
            <paged_list++ [tasks_closed] [/tasks/list] [taskListRenderer]>
              'status'   : 'closed',
              'member_id': <v++ memberID ++v>
            <++paged_list>
          </td>
        </tr>
      </table>
      <++panel>
    ++!>
  );
  putBaseFooter();
}
// ---------------------------------------------- task list --------------------------------------------------------
void CTasks::taskList()
{
  DF_STOP_IF_NO_AUTH
  if(DF_REQUEST_IS_POST)
  {
    /*
     * Исходя из значений type:
     *   member:  имеющие отношение к выбранному пользователю. Те, в которых пользователь - исполнитель, контроллер, создатель или начальник отдела (вниз по дереву)
     *            typeId - идентификатор пользователя
     *   division: имеющие отношение к подразделению
     *             typeId - идентификатор подразделения
     *   day:     все задачи на определенный день для текущего сессионного пользователя, в том числе задачи отделов под его руководством
     *
     * Списки задач исходя из значений status:
     *  open: - все открытые задачи, сюда же неназначенные задачи
     *  notassigned: нераспределенные задачи
     * open_and_not_assigned: иткрытые или нераспеределенные задачи
     * closed: закрытые задачи
     * all: все задачи
     *
     */
    DFDB_R tasks, count;
    std::string query;
    std::string firstRecord  = postValue("offset", "0");
    std::string itemsPerPage = postValue("itemsPerPage", "15");
    std::ostringstream sqlQuery;
//    std::ostringstream sqlOrder;
    std:: string fields =
        "select "
           /* 0*/ "tasks.id, "
           /* 1*/ "tasks.caption, "
           /* 2*/ "tasks.description, "
           /* 3*/ "divisions.id, "
           /* 4*/ "divisions.short_name, "
           /* 5*/ "to_char(tasks.start_timestamp, 'DD.MM.YYYY HH24:MI'), "
           /* 6*/ "to_char(tasks.control_timestamp, 'DD.MM.YYYY HH24:MI'), "
           /* 7*/ "m_creator.id, "
           /* 8*/ "m_creator.name, "
           /* 9*/ "m_performer.id, "
           /*10*/ "m_performer.name, "
           /*11*/ "m_controller.id, "
           /*12*/ "m_controller.name, "
           /*13*/ "tasks.status, "
           /*14*/ "tasks.priority, "
           /*15*/ "to_char(tasks.end_timestamp, 'DD.MM.YYYY HH24:MI'), "
           /*16*/ "divisions.manager_member_id, "
           /*17*/ "task_progress_in_percents.percents_done, "
           /*18*/ "task_progress_statuses.task_done_status, "
           /*19*/ "task_progress_statuses.task_done_description ";
    sqlQuery <<
        "from tasks "
        "left join members m_creator on m_creator.id = tasks.creator_member_id "
        "left join members m_performer on m_performer.id = tasks.performer_member_id "
        "left join members m_controller on m_controller.id = tasks.controller_member_id "
        "left join divisions on divisions.id = tasks.target_division_id "
        "left join task_progress_in_percents on task_progress_in_percents.task_id = tasks.id  "
        "left join task_progress_statuses on task_progress_statuses.task_id = tasks.id "
        "where ";
    // Обработка типа
    if(postValueExists("member_id"))
    {
      query =     "(tasks.creator_member_id    = $1 or "
                  " tasks.performer_member_id  = $1 or "
                  " tasks.controller_member_id = $1 or "
                  " tasks.id in (select task_id from task_coworkers where coworker_member_id=$1) or "
                  " tasks.target_division_id in ( select id from managed_divisions($1))) " +
                  taskListFilterTaskStatus();
      tasks = DFDB_Q("member_tasks_list_" + postValue("status"), fields + sqlQuery.str() + query + taskListOrder() + "limit $2 offset $3",
                                                                                                                  (postValue("member_id"))
                                                                                                                  (itemsPerPage)
                                                                                                                  (firstRecord));
      count = DFDB_Q("member_tasks_list_count_" + postValue("status"), "select count(tasks.id) from tasks where " + query, (postValue("member_id")));
    }
    else if(postValueExists("division_id"))
    {
      query =     "(tasks.target_division_id=$1) " +  taskListFilterTaskStatus();
      tasks = DFDB_Q("division_tasks_list_" + postValue("status"), fields + sqlQuery.str() + query + taskListOrder() + "limit $2 offset $3",
                                                                                                                  (postValue("division_id"))
                                                                                                                  (itemsPerPage)
                                                                                                                  (firstRecord));
      count = DFDB_Q("division_tasks_list_count_" + postValue("status"), "select count(tasks.id) from tasks where " + query, (postValue("division_id")));
    }
    else if(postValueExists("date"))
    {
      std::string status = postValue("status");
                  query  =   std::string("(") + DF_SQL_TEXT( (
                                                                tasks.target_division_id in (select id from managed_divisions($1)) or
                                                                tasks.performer_member_id = $1 or
                                                                tasks.controller_member_id = $1 or
                                                                tasks.creator_member_id = $1 or
                                                                (select count(task_id) from task_coworkers where coworker_member_id = $1 and task_id=tasks.id) > 0
                                                              ) and ) +
                   (status.compare("open"    ) == 0 ? "(cast($2 as date) >= cast(tasks.start_timestamp as date) and "
                                                      "(cast($2 as date) <= cast(tasks.control_timestamp as date) or "
                                                      "(cast($2 as date) <= cast(tasks.end_timestamp as date) or "
                                                      "tasks.end_timestamp is null))) " : " ") +
                   (status.compare("control" ) == 0 ? "$2 = cast(tasks.control_timestamp as date) "                     : " ") +
                   (status.compare("started" ) == 0 ? "$2 = cast(tasks.start_timestamp as date) "                       : " ") +
                   (status.compare("finished") == 0 ? "$2 = cast(tasks.end_timestamp as date) "                         : " ") +
                  ") ";
      tasks = DFDB_Q("day_list_" + status, fields + sqlQuery.str() + query + taskListOrder() + "limit $3 offset $4",
                                                                                          (DF_SESSION_MEMBER_ID_INT)
                                                                                          (postValue("date"))
                                                                                          (itemsPerPage)
                                                                                          (firstRecord));
      count = DFDB_Q("day_list_count_" + status, "select count(tasks.id) from tasks where " + query,
                                                                                          (DF_SESSION_MEMBER_ID_INT)
                                                                                          (postValue("date")));
    }
    else if(postValueExists("to_me"))
    {
      query =     "(tasks.performer_member_id = $1) " +  taskListFilterTaskStatus();
      tasks = DFDB_Q("my_tasks_list_" + postValue("status"), fields + sqlQuery.str() + query + taskListOrder() + "limit $2 offset $3",
                                                                                                              (DF_SESSION_MEMBER_ID_INT)
                                                                                                              (itemsPerPage)
                                                                                                              (firstRecord));
      count = DFDB_Q("my_tasks_list_count_" + postValue("status"), "select count(tasks.id) from tasks where " + query, (DF_SESSION_MEMBER_ID_INT));
    }
    else if(postValueExists("my_control"))
    {
      query =     "(tasks.controller_member_id = $1) " +  taskListFilterTaskStatus();
      tasks = DFDB_Q("controlled_me_tasks_list_" + postValue("status"),
                            fields + sqlQuery.str() + query + taskListOrder() +
                            "limit $2 offset $3",
                    (DF_SESSION_MEMBER_ID_INT)
                    (itemsPerPage)
                    (firstRecord));
      count = DFDB_Q("controlled_me_tasks_list_count_" + postValue("status"),
            "select count(tasks.id) from tasks where " + query,
            (DF_SESSION_MEMBER_ID_INT));
    }
    else if(postValueExists("i_am_coworker"))
    {
      query =     "(tasks.id in (select task_id from task_coworkers where coworker_member_id=$1)) " +
                  taskListFilterTaskStatus();
      tasks = DFDB_Q("i_am_coworker_tasks_list_" + postValue("status"),
                            fields + sqlQuery.str() + query + taskListOrder() +
                            "limit $2 offset $3",
                    (DF_SESSION_MEMBER_ID_INT)
                    (itemsPerPage)
                    (firstRecord));
      count = DFDB_Q("i_am_coworker_tasks_list_count_" + postValue("status"),
            "select count(tasks.id) from tasks where " + query,
            (DF_SESSION_MEMBER_ID_INT));
    }
    else if(postValueExists("i_am_creator"))
    {
      query =     "(tasks.creator_member_id=$1) " +
                  taskListFilterTaskStatus();
      tasks = DFDB_Q("i_am_creator_tasks_list_" + postValue("status"),
                            fields + sqlQuery.str() + query + taskListOrder() +
                            "limit $2 offset $3",
                    (DF_SESSION_MEMBER_ID_INT)
                    (itemsPerPage)
                    (firstRecord));
      count = DFDB_Q("i_am_creator_tasks_list_count_" + postValue("status"),
            "select count(tasks.id) from tasks where " + query,
            (DF_SESSION_MEMBER_ID_INT));
    }
    cppcms::json::value data;
    int row = 0;
    DFDB_QLOOP(tasks, task)
    {
      data["data"][row][ 0] = task[ 0].as<int>();
      data["data"][row][ 1] = task[ 1].as<std::string>("");
      data["data"][row][ 2] = task[ 2].as<std::string>("");
      data["data"][row][ 3] = task[ 3].as<int>();
      data["data"][row][ 4] = task[ 4].as<std::string>("");
      data["data"][row][ 5] = task[ 5].as<std::string>("");
      data["data"][row][ 6] = task[ 6].as<std::string>("");
      data["data"][row][ 7] = task[ 7].as<int>();
      data["data"][row][ 8] = task[ 8].as<std::string>("");
      data["data"][row][ 9] = task[ 9].as<int>();
      data["data"][row][10] = task[10].as<std::string>("");
      data["data"][row][11] = task[11].as<int>();
      data["data"][row][12] = task[12].as<std::string>("");
      data["data"][row][13] = task[13].as<std::string>("");
      data["data"][row][14] = task[14].as<std::string>("");
      data["data"][row][15] = task[15].as<std::string>("");
      data["data"][row][16] = task[16].as<int>();
      data["data"][row][17] = task[17].as<int>();
      data["data"][row][18] = task[18].as<std::string>("");
      data["data"][row][19] = task[19].as<std::string>("");
      row++;
    }
    data["totalRows"] = count[0][0].as<int>(0);
    response().out() << data;
  }
}

std::string CTasks::taskListFilterTaskStatus()
{
  std::string status = postValue("status");
  // Обработка статуса
  if(status.compare("open") == 0)
  {
    return "and (tasks.status = 'open') ";
  }
  if(status.compare("notassigned") == 0)
  {
    return "and (tasks.status = 'not_assigned') ";
  }
  if(status.compare("open_and_not_assigned") == 0)
  {
    return "and (tasks.status in ('open', 'not_assigned')) ";
  }
  if(status.compare("closed") == 0)
  {
    return "and (tasks.status = 'closed') ";
  }
  return "";
}

std::string CTasks::taskListOrder()
{
  std::string status = postValue("status");
  // Обработка статуса
  if(status.compare("open") == 0)
  {
    return "order by tasks.priority, task_progress_in_percents.percents_done desc ";
  }
  if(status.compare("notassigned") == 0)
  {
    return "order by tasks.priority ";
  }
  if(status.compare("open_and_not_assigned") == 0)
  {
    return "order by tasks.priority, task_progress_in_percents.percents_done desc ";
  }
  if(status.compare("closed") == 0)
  {
    return "order by tasks.end_timestamp desc ";
  }
  if(status.compare("all") == 0)
  {
    return "order by tasks.start_timestamp desc, tasks.control_timestamp desc ";
  }
  return "";
}
// ---------------------------------------------- task list --------------------------------------------------------
// ---------------------------------------------- description --------------------------------------------------------
void CTasks::makeEditDescription(const int &taskID,
                                 const int &taskRecordDivisionID,
                                 const std::string &text)
{
  if(CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
  {
    HTML
    (
      <!++
        <value_edit_dialog++ [<v++ taskID ++v>] [<v++ taskID ++v>] [TaskDescription] [Редактирование описания] [edit]>
          <labeled_control++ [Описание задачи] [taskDescription]>
            <textarea id='task_description_<v++ taskID ++v>'><v++ text ++v></textarea>
          <++labeled_control>
        <++value_edit_dialog>
       ++!>
    );

  }
}
// ---------------------------------------------- description --------------------------------------------------------

// ---------------------------------------------- controller --------------------------------------------------------
void CTasks::makeEditController(const int &taskID, const int &taskRecordDivisionID, const int &controllerMemberID)
{
  if(CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
  {
    HTML
    (
      <!++
        <value_edit_dialog++ [<v++ taskID ++v>] [<v++ taskID ++v>] [TaskController] [Редактирование контроля] [edit]>
          <f++ controlledMembersSelectBox(taskID, controllerMemberID); ++f>
        <++value_edit_dialog>
      ++!>
    );
  }
}

void CTasks::controlledMembersSelectBox(const int &taskID, const int &currentControllerMemberID)
{
  membersSelectBox("Контроль исполнения за", "controller", "taskController", taskID, currentControllerMemberID ? currentControllerMemberID : DF_SESSION_MEMBER_DIVISION_MANAGER_ID_INT);
}
// ---------------------------------------------- controller --------------------------------------------------------
// ---------------------------------------------- performer --------------------------------------------------------
void CTasks::makeEditPerformer(const int &taskID, const int &taskRecordDivisionID, const int &performerMemberID)
{
  if(CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
  {
    HTML
    (
      <!++
        <value_edit_dialog++ [<v++ taskID ++v>] [<v++ taskID ++v>] [TaskPerformer] [Редактирование исполнителя] [edit]>
          <f++ performerMembersSelectBox(taskID, performerMemberID); ++f>
        <++value_edit_dialog>
      ++!>
    );
  }
}

void CTasks::performerMembersSelectBox(const int &taskID, const int &currentPerformerMemberID)
{
  membersSelectBox("Ответственный исполнитель", "performer", "taskPerformer", taskID, currentPerformerMemberID ? currentPerformerMemberID : DF_SESSION_MEMBER_ID_INT);
}
// ---------------------------------------------- performer --------------------------------------------------------
// ---------------------------------------------- control date --------------------------------------------------------
void CTasks::makeEditControlDate(const int &taskID, const int &taskRecordDivisionID, const std::string &dateTime)
{
  if(CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
  {
    HTML
    (
          <!++
            <value_edit_dialog++ [<v++ taskID ++v>] [<v++ taskID ++v>] [TaskControlDate] [Редактирование даты контроля] [edit]>
              <f++ controlDateEdit(taskID, dateTime); ++f>
            <++value_edit_dialog>
          ++!>
    );
  }
}

void CTasks::controlDateEdit(const int &taskID, const std::string &currentDateTime)
{
  std::string dt = currentDateTime;
  if(dt.empty())
  {
    std::tm ts  = currentTimestamp();
    ts.tm_mday++;
    std::mktime(&ts);
    dt = std::to_string(ts.tm_mday) + "." + std::to_string(ts.tm_mon+1) + "." + std::to_string(static_cast<int>(ts.tm_year) + 1900)
                                                 + " " + std::to_string(ts.tm_hour)  + ":" + std::to_string(ts.tm_min) + ":" + std::to_string(ts.tm_sec);
  }
  dateTimeEdit("Контрольная дата", "control_date", "taskControlDate", dt, taskID);
}
// ---------------------------------------------- control date --------------------------------------------------------
// ---------------------------------------------- priority --------------------------------------------------------
void CTasks::makeEditPriority(const int &taskID, const int &taskRecordDivisionID, const std::string &priority)
{
  if(CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
  {
    HTML
    (
          <!++
            <value_edit_dialog++ [<v++ taskID ++v>] [<v++ taskID ++v>] [TaskPriority] [Редактирование приоритета] [edit]>
              <f++ prioritySelectBox(taskID, priority); ++f>
            <++value_edit_dialog>
          ++!>
    );
  }
}
// ---------------------------------------------- caption --------------------------------------------------------
void CTasks::makeEditCaption(const int &taskID, const int &taskRecordDivisionID, const std::string &caption)
{
  if(CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
  {
    HTML
    (
          <!++
            <value_edit_dialog++ [<v++ taskID ++v>] [<v++ taskID ++v>] [TaskCaption] [Редактирование заголовка] [edit]>
              <labeled_control++ [Краткое описание задачи] [taskCaption]>
                <input type="text" id='task_caption_<v++ taskID ++v>' name="caption" value='<v++ caption ++v>' />
              <++labeled_control>
            <++value_edit_dialog>
          ++!>
    );
  }
}
// ---------------------------------------------- caption --------------------------------------------------------
// ---------------------------------------------- division --------------------------------------------------------
void CTasks::makeEditDivision(const int &taskID, const int &taskRecordDivisionID, const int &divisionID)
{
  if(CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
  {
    HTML
    (
          <!++
            <value_edit_dialog++ [<v++ taskID ++v>] [<v++ taskID ++v>] [TaskDivision] [Редактирование подразделения] [edit]>
              <f++ divisionsSelectBox(<s++Задача для подразделения++s>, <s++taskDivision++s>, taskID, divisionID); ++f>
            <++value_edit_dialog>
          ++!>
    );
  }
}
// ---------------------------------------------- division --------------------------------------------------------

void CTasks::prioritySelectBox(const int &taskID, std::string currentPriority)
{
  std::string selectID = "task_priority" + (taskID == 0 ? "" : "_" + std::to_string(taskID));
  std::string where = "";
  std::string selected = "";
  DFDB_R priorityes = DFDB_Q("status_priority_box",
               "select "
                  "e.enumlabel, "
                  "task_priorityes_to_name(cast(e.enumlabel as task_priorityes)) "
                "from pg_enum e "
                "left join pg_type t ON e.enumtypid = t.oid "
                "where t.typname = 'task_priorityes'", /* empty */);
  HTML
  (
     <!++
        <labeled_control++ [Приоритет] [taskPriority]>
        <select id="<v++ selectID ++v>" name="priority">
     ++!>
  );
  DFDB_QLOOP(priorityes, priority)
  {
    selected = currentPriority.compare(priority[0].as<std::string>()) == 0 ? "selected" : "";
    HTML
    (
       <!++
          <option value='<v++ priority[0].as<std::string>() ++v>' <v++ selected ++v>><v++ priority[1].as<std::string>() ++v></option>
       ++!>
    );
  }
  HTML(<!++ </select><++labeled_control> ++!>);
}

// ---------------------------------------------- priority --------------------------------------------------------
void CTasks::addTask()
{
  if(DF_REQUEST_IS_POST)
  {
    DF_STOP_IF_NO_AUTH;
    int controllerID = postValue("controller_id", 0);
    int performerID = postValue("performer_id", 0);
    std::string status = performerID ? "open" : "not_assigned";
    DFDB_R newtask = DFDB_Q("new_task", "insert into tasks ("
                                "caption, "
                                "description, "
                                "target_division_id, "
                                "control_timestamp, "
                                "creator_member_id, "
                                "controller_member_id, "
                                "performer_member_id, "
                                "priority,"
                                "status"
                              ") values ($1, $2, $3, $4, $5, $6, $7, $8, $9) returning id",
                               (postValue("caption"))
                               (postValue("description"))
                               (postValue("division_id"))
                               (postValue("control_date"))
                               (DF_SESSION_MEMBER_ID_INT)
                               (controllerID)
                               (performerID)
                               (postValue("priority"))
                               (status));
    int taskID = CMS_SQL_LAST_INSERT_ID(newtask);
    DFDB_R owner = DFDB_Q("member_name", "select name from members where id=$1", (DF_SESSION_MEMBER_ID_INT));
    DFDB_R pri   = DFDB_Q("task_priority_text", "select task_priorityes_to_name($1)", (postValue("priority")));
    updateChangelog(taskID, "owner_member_changed",
                    "Задаче <a href='/tasks/show/" + std::to_string(taskID) + "'>" + postValue("caption") + "</a> "
                    "назначен владелец <a href='/members/show/" + DF_SESSION_MEMBER_ID_STR + "'>" + owner[0][0].as<std::string>() + "</a>");
    updateChangelog(taskID, "control_date_changed",
                    "Задаче <a href='/tasks/show/" + std::to_string(taskID) + "'>" + postValue("caption") + "</a> установлена контрольная дата "
                     + postValue("control_date"));
    updateChangelog(taskID, "priority_changed",
                    "Задаче <a href='/tasks/show/" + std::to_string(taskID) + "'>" + postValue("caption") + "</a> установлен приоритет "
                     + pri[0][0].as<std::string>() );
    if(controllerID)
    {
      DFDB_R controller = DFDB_Q("member_name", "select name from members where id=$1", (controllerID));
      updateChangelog(taskID, "control_member_changed",
                      "Задаче <a href='/tasks/show/" + std::to_string(taskID) + "'>" + postValue("caption") + "</a> "
                      "назначен контроллер <a href='/members/show/" + postValue("controller_id") + "'>" + controller[0][0].as<std::string>() + "</a>");
    }
    if(performerID)
    {
      DFDB_R performer = DFDB_Q("member_name", "select name from members where id=$1", (performerID));
      updateChangelog(taskID, "performer_member_changed",
                      "Задаче <a href='/tasks/show/" + std::to_string(taskID) + "'>" + postValue("caption") + "</a> "
                      "назначен исполнитель <a href='/members/show/" + postValue("controller_id") + "'>" + performer[0][0].as<std::string>() + "</a>");
      updateChangelog(taskID, "opened", "Задача <a href='/tasks/show/" + std::to_string(taskID) + "'>" + postValue("caption") + "</a> создана");
    }
    else
    {
      updateChangelog(taskID, "opened", "Задача <a href='/tasks/show/" + std::to_string(taskID) + "'>" + postValue("caption") + "</a> создана без назначения исполнителя");
    }
    response().set_redirect_header("/tasks/", cppcms::http::response::see_other);
  }
  else
  {
    putBaseHeader();
    HTML
    (
      <!++
      <panel++ [Добавление задачи] [ ] [visible] [taskadd]>
       <form method='POST' action='/tasks/add'>
         <table class='form'>
         <tr>
           <td>
               <labeled_control++ [Краткое описание задачи] [taskCaption]>
               <input type="text" id="id_caption" name="caption"/>
               <++labeled_control>
           </td>
           <td><f++ prioritySelectBox(); ++f></td>
           <td><f++ controlDateEdit(); ++f></td>
           <td><f++ divisionsSelectBox(<s++ Задача для подразделения ++s>, <s++ taskDivision ++s>); ++f></td>
           <td><f++ controlledMembersSelectBox(); ++f></td>
           <td><f++ performerMembersSelectBox(); ++f></td>
         </tr>
         </table>
         <labeled_control++ [Описание задачи] [taskDescription]>
         <textarea id="id_description" name="description"/></textarea>
         <++labeled_control>
         <input type="submit" value="Создать задачу" />
       </form>
      <++panel>
      ++!>
    );
    putBaseFooter();
  }
}

void CTasks::editTask(std::string id)
{
  if(DF_REQUEST_IS_POST)
  {
    DF_STOP_IF_NO_AUTH;
    DFDB_R task = DFDB_Q("task_caption", "select caption from tasks where id=$1", (id));
    std::string changelogComment;
    int taskID = atoi(id.c_str());
    if(postValueIs("what", "description"))
    {
      DFDB_R description = DFDB_Q("task_description", "select description from tasks where id=$1", (id));
      DFDB_Q("task_description_update", "update tasks set description=$1 where id=$2", (postValue("description"))(id));
      changelogComment = updateChangelog(taskID, "description_edited",
                      "Задаче <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> изменено описание с "
                      "<div class='task_description'>" + description[0][0].as<std::string>() + "</div> на"
                      "<div class='task_description'>" + postValue("description") + "</div>");
    }
    else if(postValueIs("what", "controller"))
    {
      DFDB_R controllerOld = DFDB_Q("task_controller_name_from_task_id",
                                        "select id, name from members where id=(select controller_member_id from tasks where id=$1)",(id));
      DFDB_R controllerNew = DFDB_Q("member_name", "select name from members where id=$1", (postValue("controller_id")));
      DFDB_Q("task_controller_update", "update tasks set controller_member_id=$1 where id=$2",(postValue("controller_id"))(id));
      changelogComment = updateChangelog(taskID, "control_member_changed",
                      "Задаче <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> изменен контроллер с "
                      "<a href='/members/show/" + controllerOld[0][0].as<std::string>() + "'>" + controllerOld[0][1].as<std::string>() + "</a> на "
                      "<a href='/members/show/" + postValue("controller_id") + "'>" + controllerNew[0][0].as<std::string>() + "</a>");
    }
    else if(postValueIs("what", "performer"))
    {
      DFDB_R performerOld = DFDB_Q("task_performer_name_from_task_id",
                                       "select id, name from members where id=(select performer_member_id from tasks where id=$1)",(id));
      DFDB_R performerNew = DFDB_Q("member_name", "select name from members where id=$1", (postValue("performer_id")));
      DFDB_Q("task_performer_update", "update tasks set performer_member_id=$1 where id=$2",(postValue("performer_id"))(id));
      changelogComment = updateChangelog(taskID, "performer_member_changed",
                      "Задаче <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> изменен исполнитель с "
                      "<a href='/members/show/" + performerOld[0][0].as<std::string>() + "'>" + performerOld[0][1].as<std::string>() + "</a> на "
                      "<a href='/members/show/" + postValue("performer_id") + "'>" + performerNew[0][0].as<std::string>() + "</a>");
      DFDB_R perfUpdSt = DFDB_Q("task_performer_status_update", "update tasks set status='open' where id=$1 and status='not_assigned'",(id));
      if(perfUpdSt.affected_rows())
      {
        changelogComment += updateChangelog(taskID, "opened",
                        "Задача <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> открыта в связи с назначением исполнителя "
                        "<a href='/members/show/" + postValue("performer_id") + "'>" + performerNew[0][0].as<std::string>() + "</a>");
      }
    }
    else if(postValueIs("what", "control_date"))
    {
      DFDB_R cdate = DFDB_Q("task_control_date", "select to_char(tasks.control_timestamp, 'DD.MM.YYYY HH24:MI') from tasks where id=$1", (id));
      DFDB_Q("task_control_date_update", "update tasks set control_timestamp=$1 where id=$2", (postValue("control_date"))(id));
      changelogComment = updateChangelog(taskID, "control_date_changed",
                      "Задаче <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> изменена контроляная дата с "
                       + cdate[0][0].as<std::string>() + " на "
                       + postValue("control_date"));
    }
    else if(postValueIs("what", "priority"))
    {
      DFDB_R priOld = DFDB_Q("task_priority_from_table", "select task_priorityes_to_name(priority) from tasks where id=$1", (id));
      DFDB_R priNew = DFDB_Q("task_priority_text", "select task_priorityes_to_name($1)", (postValue("priority")));
      DFDB_Q("task_priority_update", "update tasks set priority=$1 where id=$2", (postValue("priority"))(id));
      changelogComment = updateChangelog(taskID, "priority_changed",
                      "Задаче <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> изменен приоритет с "
                       + priOld[0][0].as<std::string>() + " на " + priNew[0][0].as<std::string>() );
    }
    else if(postValueIs("what", "caption"))
    {
      DFDB_Q("task_caption_update", "update tasks set caption=$1 where id=$2", (postValue("caption"))(id));
      changelogComment = updateChangelog(taskID, "caption_edited",
                      "Задаче <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> изменен заголовок на "
                      "<a href='/tasks/show/" + id + "'>" + postValue("caption") + "</a>");
    }
    else if(postValueIs("what", "division"))
    {
      DFDB_R divisionOld = DFDB_Q("task_division_name_from_task_id",
                                        "select id, name from divisions where id=(select target_division_id from tasks where id=$1)",(id));
      DFDB_R divisionNew = DFDB_Q("division_name", "select name from divisions where id=$1", (postValue("division_id")));
      DFDB_Q("task_division_update", "update tasks set target_division_id=$1 where id=$2", (postValue("division_id"))(id));
      changelogComment = updateChangelog(taskID, "division_changed",
                      "Задаче <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> изменено целевое подразделение с "
                      "<a href='/divisions/show/" + divisionOld[0][0].as<std::string>() + "'>" + divisionOld[0][1].as<std::string>() + "</a> на "
                      "<a href='/divisions/show/" + postValue("division_id") + "'>" + divisionNew[0][0].as<std::string>() + "</a>");
    }
    else if(postValueIs("what", "complete"))
    {
      DFDB_Q("task_to_complete_update", "update tasks set status='closed', end_timestamp=now() where id=$1", (id));
      changelogComment = updateChangelog(taskID, "closed",
                      "Задача <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> помечена, как исполненная");
      CMS_RIGHTS->clearTask(DF_SESSION_MEMBER_ID_INT, taskID);
      response().set_redirect_header("/tasks/show/" + id, cppcms::http::response::see_other);
    }
    else if(postValueIs("what", "reopen"))
    {
      DFDB_Q("task_to_reopen_update", "update tasks set status='open', end_timestamp=NULL where id=$1", (id));
      changelogComment = updateChangelog(taskID, "opened",
                      "Задача <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> открыта заново");
      CMS_RIGHTS->clearTask(DF_SESSION_MEMBER_ID_INT, taskID);
      response().set_redirect_header("/tasks/show/" + id, cppcms::http::response::see_other);
    }
    else if(postValueIs("what", "coworker"))
    {
      DFDB_R coworker = DFDB_Q("member_name", "select name from members where id=$1", (postValue("coworker_id")));
      if(postValueIs("operation", "add"))
      {
        DFDB_Q("add_coworker", "insert into task_coworkers (task_id, coworker_member_id) values($1, $2)", (id)(postValue("coworker_id")));
        changelogComment = updateChangelog(taskID, "coworker_added",
                        "К задаче <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> добавлен исполнитель " +
                        "<a href='/members/show/" + postValue("coworker_id") + "'>" + coworker[0][0].as<std::string>() + "</a>");
      }
      else if(postValueIs("operation", "rm"))
      {
        DFDB_Q("rm_coworker", "delete from task_coworkers where task_id=$1 and coworker_member_id=$2", (id)(postValue("coworker_id")));
        changelogComment = updateChangelog(taskID, "coworker_removed",
                        "У задачи <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> удален исполнитель " +
                        "<a href='/members/show/" + postValue("coworker_id") + "'>" + coworker[0][0].as<std::string>() + "</a>");
      }
    }

    if(!postValueIsEmpty("comment"))
    {
      addComment(id,
        postValue("comment") +
          "<div class='changelog_comment'>Изменено:<br />" + changelogComment + "</div>",
        false, false);
    }
  }
  else
  {
    response().set_redirect_header("/tasks/", cppcms::http::response::see_other);
  }
}

void CTasks::showTask(std::string id)
{

  DFDB_R task = DFDB_Q("task", DF_SQL_TEXT(select
                 /* 0*/ tasks.id,
                 /* 1*/ tasks.caption,
                 /* 2*/ tasks.description,
                 /* 3*/ divisions.id,
                 /* 4*/ divisions.short_name,
                 /* 5*/ to_char(tasks.start_timestamp, 'DD.MM.YYYY HH24:MI'),
                 /* 6*/ to_char(tasks.control_timestamp, 'DD.MM.YYYY HH24:MI'),
                 /* 7*/ m_creator.id,
                 /* 8*/ m_creator.name,
                 /* 9*/ m_performer.id,
                 /*10*/ m_performer.name,
                 /*11*/ m_controller.id,
                 /*12*/ m_controller.name,
                 /*13*/ tasks.status,
                 /*14*/ tasks.priority,
                 /*15*/ to_char(tasks.end_timestamp, 'DD.MM.YYYY HH24:MI'),
                 /*16*/ divisions.manager_member_id,
                 /*17*/ task_priorityes_to_name(tasks.priority),
                 /*18*/ cast(EXTRACT(EPOCH FROM age(tasks.control_timestamp, tasks.start_timestamp)) as int),
                 /*19*/ cast(EXTRACT(EPOCH FROM age(now(), tasks.start_timestamp)) as int),
                 /*20*/ task_timeranges.overtime_control_time,
                 /*21*/ task_timeranges.total_task_from_start_to_control_time,
                 /*22*/ task_timeranges.left_task_from_start_to_now_time,
                 /*23*/ task_timeranges.remaining_task_from_now_to_control_time,
                 /*24*/ task_timeranges.total_task_from_control_to_end_time,
                 /*25*/ task_timeranges.total_task_from_start_to_end_time,
                 /*26*/ task_progress_in_percents.percents_done,
                 /*27*/ task_progress_statuses.task_done_status,
                 /*28*/ task_progress_statuses.task_done_description,
                 /*29*/ divisions.name
              from tasks
              left join members m_creator on m_creator.id = tasks.creator_member_id
              left join members m_performer on m_performer.id = tasks.performer_member_id
              left join members m_controller on m_controller.id = tasks.controller_member_id
              left join divisions on divisions.id = tasks.target_division_id
              left join task_timeranges on task_timeranges.task_id = tasks.id
              left join task_progress_in_percents on task_progress_in_percents.task_id = tasks.id
              left join task_progress_statuses on task_progress_statuses.task_id = tasks.id
              where tasks.id=$1),
              (id));
  putBaseHeader();
  if (task.size())
  {
    int taskID = task[0][0].as<int>();
    HTML
    (
      <!++
         <panel++ [<v++ task[0][1].as<std::string>() ++v>] [ ] [visible] [taskshow]>
         <div class='task'>
            <div class='task_description'>
              <span id='task_description_text_<v++ taskID ++v>'><v++ task[0][2].as<std::string>() ++v><help++ [taskDescription]></span>
              <f++ makeEditDescription(taskID, task[0][3].as<int>(), task[0][2].as<std::string>()); ++f>
            </div>
            <table class='task_table'>


              <tr>
                  <th>Идентификатор<help++ [taskID]></th>
                  <th>Статус<help++ [taskStatus]></th>
                  <th>Заголовок<help++ [taskCaption]></th>
                  <th>Подразделение<help++ [taskDivision]></th></tr>
              <tr>
                <td><v++ taskID ++v></td>
                <td>
                  <v++ task[0][13].as<std::string>() ++v>
                </td>
                <td>
                  <v++ task[0][1].as<std::string>() ++v>
                  <f++ makeEditCaption(taskID, task[0][3].as<int>(), task[0][1].as<std::string>()); ++f>
                </td>
                <td>
                  <a href='/divisions/show/<v++ task[0][3].as<int>() ++v>'><v++ task[0][29].as<std::string>() ++v> (<v++ task[0][4].as<std::string>() ++v>)</a>
                  <f++ makeEditDivision(taskID, task[0][3].as<int>(), task[0][3].as<int>()); ++f>
                </td>
              </tr>


              <tr>
                <th>Создатель<help++ [taskCreator]></th>
                <th>Контролирует<help++ [taskController]></th>
                <th>Ответственный исполнитель<help++ [taskPerformer]></th>
                <th>Рабочая группа</th>
              </tr>
              <tr>
                <td><a href='/members/show/<v++ task[0][7].as<std::string>() ++v>'><v++ task[0][8].as<std::string>() ++v></a></td>
                <td>
                  <a href='/members/show/<v++ task[0][11].as<std::string>() ++v>'><v++ task[0][12].as<std::string>() ++v></a>
                  <f++ makeEditController(taskID, task[0][3].as<int>(), task[0][11].as<int>()); ++f>
                </td>
                <td>
                  <a href='/members/show/<v++ task[0][9].as<std::string>(std::string()) ++v>'><v++ task[0][10].as<std::string>(std::string()) ++v></a>
                  <f++ makeEditPerformer(taskID, task[0][3].as<int>(), task[0][9].as<int>(0)); ++f>
                </td>
                <td>
                  <f++ listTaskCoworkers(taskID, task[0][3].as<int>()); ++f>
                </td>
                </td>
              </tr>


              <tr>
                <th>Дата создания<help++ [taskCreationDate]></th>
                <th>Дата контроля<help++ [taskControlDate]></th>
                <th>Дата закрытия<help++ [taskCloseDate]></th>
                <th></th>
              </tr>
              <tr>
                <td><v++ task[0][5].as<std::string>() ++v></td>
                <td>
                  <v++ task[0][6].as<std::string>() ++v>
                  <f++ makeEditControlDate(taskID, task[0][3].as<int>(), task[0][6].as<std::string>()); ++f>
                </td>
                <td><v++ task[0][15].as<std::string>(std::string()) ++v></td>
                <td></td>
              </tr>


              <tr>
                  <th>Времени на задачу<help++ [taskTotalTime]></th>
                  <th>Прошло с момента старта задачи<help++ [taskTimeFromStart]></th>
                  <th><v++ (task[0][20].as<bool>() ?
                      <s++ Прошло после контрольного срока <help++ [taskTimeFromEnd]> ++s> :
                      <s++ Осталось до контрольного срока <help++ [taskTimeToEnd]> ++s>) ++v></th>
                  <th></th>
              </tr>
              <tr>
                <td><v++ formatPgTimeInterval(task[0][21].as<std::string>()) ++v></td>
                <td><v++ formatPgTimeInterval(task[0][22].as<std::string>()) ++v></td>
                <td><v++ formatPgTimeInterval(task[0][23].as<std::string>()) ++v></td>
                <td></td>
              </tr>
              <tr>
                <th>Приоритет<help++ [taskPriority]></th>
                <th colspan='3'>Прогресс<help++ [taskProgress]></th></tr>
              <tr>
                <td>
                 <div class='priority_<v++ task[0][14].as<std::string>() ++v>'><v++ task[0][17].as<std::string>() ++v></div>
                 <f++ makeEditPriority(taskID, task[0][3].as<int>(), task[0][14].as<std::string>()); ++f>
                </td>
                <td colspan='3'>
                  <span class='progress'>
                    <span style='width: <v++ task[0][26].as<std::string>() ++v>%;'
                          class='full_profile <v++ task[0][27].as<std::string>() ++v>'
                          title='<v++ task[0][28].as<std::string>() ++v>'>
                      <v++ task[0][26].as<std::string>() ++v>%
                    </span>
                  </span>
                </td>
              </tr>


            </table>
         </div>
        <++panel>
        <tabcontrol++ [info]>
          <tabpage++ [Комментарии] [info] [info_comments] [tabCommentView]>
            <tabcontrol++ [comments]>
              <tabpage++ [Просмотр] [comments] [comments_view] [tabCommentView]>
                <div id='task_comments'></div> <thread_load++ ['/tasks/comments/<v++ taskID ++v>'] [task_comments]>
              <++tabpage>
              <f++ makeCommentForm(taskID, task[0][3].as<int>(), task[0][13].as<std::string>()); ++f>
            <++tabcontrol>
          <++tabpage>
          <tabpage++ [Файлы] [info] [info_files] [taskFiles]>
          <tabcontrol++ [files]>
            <tabpage++ [Просмотр] [files] [files_view] [taskFiles]>
             <div id='uploadedfiles'></div> <thread_load++ ['/file/list/task/<v++ taskID ++v>'] [uploadedfiles]>
            <++tabpage>
            <f++ makeTaskUploadDialog(taskID, task[0][3].as<int>(), task[0][13].as<std::string>()); ++f>
          <++tabcontrol>
          <++tabpage>
          <tabpage++ [История] [info] [info_history] [tabHistory]>
             <paged_list++ [task_cl] [/tasks/changelog/<v++ taskID ++v>] [taskChangelogRenderer]> <++paged_list>
          <++tabpage>
          <f++ makeTaskControls(taskID, task[0][3].as<int>(), task[0][13].as<std::string>()); ++f>
        <++tabcontrol>

      ++!>
    );
  }
  putBaseFooter();
}

void CTasks::listTaskCoworkers(const int &taskID, const int &taskRecordDivisionID)
{
  DFDB_R coworkers = DFDB_Q("task_coworkers", DF_SQL_TEXT(
              select
              /* 0*/ members.id,
              /* 1*/ members.name,
              /* 2*/ divisions.id,
              /* 3*/ divisions.short_name
              from task_coworkers
              left join members on members.id=task_coworkers.coworker_member_id
              left join divisions on divisions.id=members.division_id
              where
              task_coworkers.task_id=$1),
              (taskID));
  std::vector<int> ids;
  bool editable = CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT);
  if(coworkers.size())
  {
    int coworkerID;
    DFDB_QLOOP(coworkers, coworker)
    {
      coworkerID = coworker[0].as<int>();
      ids.push_back(coworkerID);
      HTML
      (
        <!++
          <div class='performer'>
            <a href='/members/show/<v++ coworkerID ++v>'><v++ coworker[1].as<std::string>() ++v></a>&nbsp;
           (<a href='/divisions/show/<v++ coworker[2].as<std::string>() ++v>'><v++ coworker[3].as<std::string>() ++v></a>)
        ++!>
      );
      if(editable)
      {
        HTML
        (<!++
          <value_edit_dialog++ [<v++ coworkerID ++v>] [<v++ taskID ++v>] [TaskCoworkerRemove] [Удаление исполнителя] [close-round]>
          <input type='hidden' id='task_coworker_rm_id_<v++ taskID ++v>' value='<v++ coworker[0].as<std::string>() ++v>'/>
          <++value_edit_dialog>
        ++!>);

      }
      HTML(<!++ </div> ++!>);
    }
  }
  if(editable)
  {
    HTML
    (
          <!++
            <value_edit_dialog++ [add] [<v++ taskID ++v>] [TaskCoworkerAdd] [Добавление исполнителей] [plus-round]>
              <f++ membersSelectBox(<s++Исполнители++s>, <s++coworker++s>, <s++taskCoworker++s>, taskID, ids, true); ++f>
            <++value_edit_dialog>
          ++!>
    );
  }
  else
  {
    HTML(<!++ Нет ++!>);
  }
}


void df::ui::CTasks::dayTotal(std::string year, std::string month, std::string day)
{
  DF_STOP_IF_NO_AUTH;
//  DFDB_R tasksCounts = DFDB_Q("short_day_task_counts", DF_SQL_TEXT(select
//                        'В работе' as today_opened_title, sum((
//                          select count(tasks_open.id)
//                          from tasks tasks_open
//                          where
//                            tasks_open.target_division_id=divisions.id and
//                            (cast($1 as date) >= cast(tasks_open.start_timestamp as date) and
//                            (cast($1 as date) <= cast(tasks_open.control_timestamp as date) or
//                            (cast($1 as date) <= cast(tasks_open.end_timestamp as date) or
//                            tasks_open.end_timestamp is null)))
//              /* 0*/       )) as today_opened,
//                        'Контроль' as today_control_title, sum((
//                          select count(tasks_control.id)
//                          from tasks tasks_control
//                          where
//                            tasks_control.target_division_id=divisions.id and
//                            $1 = cast(tasks_control.control_timestamp as date)
//              /* 1*/       )) as today_control,
//                        'Завершено' as today_closed_title, sum((
//                          select count(tasks_closed.id)
//                          from tasks tasks_closed
//                          where
//                            tasks_closed.target_division_id=divisions.id and
//                            $1 = cast(tasks_closed.end_timestamp as date)
//              /* 2*/       )) as today_closed,
//                        'Запущено' as today_started_title, sum((
//                          select count(tasks_started.id)
//                          from tasks tasks_started
//                          where
//                            tasks_started.target_division_id=divisions.id and
//                            $1 = cast(tasks_started.start_timestamp as date)
//              /* 3*/       )) as today_started
//                      from divisions
//                      where
//                        divisions.id in (select id from managed_divisions($2)) or divisions.id in (select members.division_id from members where id=$2) ),
//              (year+"-"+month+"-"+day)(DF_SESSION_MEMBER_ID_INT));

  DFDB_R tasksCounts = DFDB_Q("short_day_task_counts", DF_SQL_TEXT(select
                              'В работе' as today_opened_title, sum((
                                select count(tasks_open.id)
                                from tasks tasks_open
                                where
                                  (
                                    tasks_open.target_division_id in (select id from managed_divisions($2)) or
                                    tasks_open.performer_member_id = $2 or
                                    tasks_open.controller_member_id = $2 or
                                    tasks_open.creator_member_id = $2 or
                                    (select count(task_id) from task_coworkers where coworker_member_id = $2 and task_id=tasks_open.id) > 0
                                  ) and
                                  (cast($1 as date) >= cast(tasks_open.start_timestamp as date) and
                                  (cast($1 as date) <= cast(tasks_open.control_timestamp as date) or
                                  (cast($1 as date) <= cast(tasks_open.end_timestamp as date) or
                                  tasks_open.end_timestamp is null)))
                 /* 0*/       )) as today_opened,
                              'Контроль' as today_control_title, sum((
                                select count(tasks_control.id)
                                from tasks tasks_control
                                where
                                  (
                                    tasks_control.target_division_id in (select id from managed_divisions($2)) or
                                    tasks_control.performer_member_id = $2 or
                                    tasks_control.controller_member_id = $2 or
                                    tasks_control.creator_member_id = $2 or
                                    (select count(task_id) from task_coworkers where coworker_member_id = $2 and task_id=tasks_control.id) > 0
                                  ) and
                                  $1 = cast(tasks_control.control_timestamp as date)
                 /* 1*/       )) as today_control,
                              'Завершено' as today_closed_title, sum((
                                select count(tasks_closed.id)
                                from tasks tasks_closed
                                where
                                  (
                                    tasks_closed.target_division_id in (select id from managed_divisions($2)) or
                                    tasks_closed.performer_member_id = $2 or
                                    tasks_closed.controller_member_id = $2 or
                                    tasks_closed.creator_member_id = $2 or
                                    (select count(task_id) from task_coworkers where coworker_member_id = $2 and task_id=tasks_closed.id) > 0
                                  ) and
                                  $1 = cast(tasks_closed.end_timestamp as date)
                 /* 2*/       )) as today_closed,
                              'Запущено' as today_started_title, sum((
                                select count(tasks_started.id)
                                from tasks tasks_started
                                where
                                  (
                                    tasks_started.target_division_id in (select id from managed_divisions($2)) or
                                    tasks_started.performer_member_id = $2 or
                                    tasks_started.controller_member_id = $2 or
                                    tasks_started.creator_member_id = $2 or
                                    (select count(task_id) from task_coworkers where coworker_member_id = $2 and task_id=tasks_started.id) > 0
                                  ) and
                                  $1 = cast(tasks_started.start_timestamp as date)
               /* 3*/       )) as today_started
                            from (values('')) empty ),
              (year+"-"+month+"-"+day)(DF_SESSION_MEMBER_ID_INT));
  if (tasksCounts.size())
  {
    if(tasksCounts[0][1].as<int>(0) > 0 ||
       tasksCounts[0][3].as<int>(0) > 0 ||
       tasksCounts[0][5].as<int>(0) > 0 ||
       tasksCounts[0][7].as<int>(0) > 0)
    {
      HTML(<!++ <div class='title'>Задачи</div> ++!>);
      for(int i=0; i < 8; i+=2)
      {
        //CMS_DEBUG_LOG(tasksCounts[0][i].name());
        if(tasksCounts[0][i+1].as<int>(0) > 0)
        {
          HTML
          (
            <!++
              <div class='today <v++ tasksCounts[0][i+1].name() ++v>'>
                <table class='task_stat_counts'>
                  <tr>
                    <td><v++ tasksCounts[0][i].as<std::string>() ++v></td>
                    <td><v++ tasksCounts[0][i+1].as<int>(0) ++v></td>
                  </tr>
                </table>
              </div>
            ++!>
          );
        }
      }
    }
  }
}

void CTasks::makeTaskUploadDialog(const int &taskID, const int &taskRecordDivisionID, const std::string &taskStatus)
{
  if(taskStatus.compare("open") == 0 && CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT)) // Загрузка файлов
  {
    HTML
    (
       <!++
          <tabpage++ [Добавление] [files] [files_add] [tabAddFiles]>
            <div id='task_file_upload'>
                    <script type='text/javascript'>
                      initUpload('task_file_upload', '/file/upload/task/<v++ taskID ++v>');
                    </script>
            </div>
          <++tabpage>
       ++!>
    );
  }
}

void CTasks::makeTaskControls(const int &taskID, const int &taskRecordDivisionID, const std::string &taskStatus)
{
  if(CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT) ||
     CMS_RIGHTS->checkTask(taskID, df::rights::rtCanAdd , taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT)) // Установка статуса задачи
  {
    HTML(<!++ <tabpage++ [Управление] [info] [info_control] [tabControl]> ++!>);
    if(taskStatus.compare("open") == 0 && CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
    {
      HTML
      (
         <!++
          <labeled_control++ [Завершение задачи] [taskDone]>
            <form method='POST' action='/tasks/edit/<v++ taskID ++v>'>
              <input type="hidden" name='what' value="complete" />
              <input type="submit" value="Задача выполнена!" />
            </form>
          <++labeled_control>
         ++!>
      );
    }
    else if(taskStatus.compare("closed") == 0 && CMS_RIGHTS->checkTask(taskID, df::rights::rtCanAdd, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
    {
      HTML
      (
         <!++
          <labeled_control++ [Возврат в работу] [taskDoneNot]>
            <form method='POST' action='/tasks/edit/<v++ taskID ++v>'>
              <input type="hidden" name='what' value="reopen" />
              <input type="submit" value="Задача не выполнена, открыть заново." />
            </form>
          <++labeled_control>
         ++!>
      );
    }
    HTML(<!++ <++tabpage> ++!>);
  }
}

void CTasks::showChangelog(std::string id)
{
  DF_STOP_IF_NO_AUTH
  if(DF_REQUEST_IS_POST)
  {
    int firstRecord  = postValue("offset", 0);
    int itemsPerPage = postValue("itemsPerPage", 15);
    DFDB_R taskChangelog = DFDB_Q("task_changelog",
                "select "
                 /* 0*/ "changelog.id,"
                 /* 1*/ "to_char(changelog.change_timestamp, 'DD.MM.YYYY HH24:MI'),"
                 /* 2*/ "changelog.what_changed,"
                 /* 3*/ "changelog.description,"
                 /* 4*/ "members.id,"
                 /* 5*/ "members.name,"
                 /* 6*/ "changelog_type_to_name(changelog.what_changed) "
                "from changelog "
                "left join members on changelog.changer_member_id = members.id "
                "where "
                  "changelog.class_of_change='task' and "
                  "changelog.class_id=$1 "
                  "order by changelog.change_timestamp desc "
                  "limit $2 offset $3",
                (id)(itemsPerPage)(firstRecord));
    cppcms::json::value data;
    int row = 0;
    DFDB_QLOOP(taskChangelog, entry)
    {
      data["data"][row][ 0] = entry[ 0].as<int>();
      data["data"][row][ 1] = entry[ 1].as<std::string>("");
      data["data"][row][ 2] = entry[ 2].as<std::string>("");
      data["data"][row][ 3] = entry[ 3].as<std::string>();
      data["data"][row][ 4] = entry[ 4].as<int>();
      data["data"][row][ 5] = entry[ 5].as<std::string>("");
      data["data"][row][ 6] = entry[ 6].as<std::string>("");
      row++;
    }
    DFDB_R count = DFDB_Q("task_changelog_count",
                          "select count(changelog.class_id) "
                          "from changelog "
                          "left join members on changelog.changer_member_id = members.id "
                          "where "
                            "changelog.class_of_change='task' and "
                            "changelog.class_id=$1 ",
                          (id));
    data["totalRows"] = count[0][0].as<int>(0);
    response().out() << data;
  }
}
// +++++++++++++++++++++++++++++++++++ Комментарии ++++++++++++++++++++++++++++++++
void CTasks::addComment(std::string id)
{
  DF_STOP_IF_NO_AUTH;
  if(DF_REQUEST_IS_POST)
  {
    addComment(id, postValue("comment"), true, true);
  }
}

void CTasks::addComment(const std::string &id, const std::string &comment, const bool &redirect, const bool &addChangelogRecord)
{
  DFDB_R newComment = DFDB_Q("new_comment", "insert into comments ("
                              "comment, "
                              "owner_id, "
                              "object_class, "
                              "class_id"
                            ") values ($1, $2, $3, $4) returning id",
                             (comment)
                             (DF_SESSION_MEMBER_ID_INT)
                             ("task")
                             (id));
  if(addChangelogRecord)
  {
    int commentID = CMS_SQL_LAST_INSERT_ID(newComment);
    std::string str_commentID = std::to_string(commentID);
    DFDB_R task = DFDB_Q("task_caption", "select caption from tasks where id=$1", (id));
    updateChangelog(atoi(id.c_str()), "comment_added",
                    "Задаче <a href='/tasks/show/" + id + "'>" + task[0][0].as<std::string>() + "</a> "
                    "добавлен комментарий <a href='/tasks/show/" + id + "#comment_" + str_commentID + "'>#" + str_commentID+ "</a>");
  }
  if(redirect) { response().set_redirect_header("/tasks/show/" + id, cppcms::http::response::see_other); }
}

void CTasks::listComments(std::string id)
{
  DF_STOP_IF_NO_AUTH;
  DFDB_R comments = DFDB_Q("task_comments",
                       "select "
                       /* 0*/ "comments.id, "
                       /* 1*/ "comments.comment, "
                       /* 2*/ "to_char(comments.comment_timestamp, 'DD.MM.YYYY HH24:MI'), "
                       /* 3*/ "members.id, "
                       /* 4*/ "members.name "
                       "from comments "
                       "left join members on members.id = comments.owner_id "
                       "where comments.object_class='task' and comments.class_id=$1 "
                       "order by comments.comment_timestamp desc ",
                       (id));
  if (comments.size())
  {
    DFDB_QLOOP(comments, comment)
    {
      HTML
      (
        <!++
            <div class='comment'>
              <a name='comment_<v++ comment[0].as<std::string>() ++v>' />
              <div class='comment_caption'>
                <v++ comment[2].as<std::string>() ++v> :: <a href='/members/show/<v++ comment[3].as<std::string>() ++v>'><v++ comment[4].as<std::string>() ++v></a>
              </div>
              <div class='comment_body'><v++ comment[1].as<std::string>() ++v></div>
            </div>
        ++!>
      );
    }
  }
}

void CTasks::makeCommentForm(const int &taskID, const int &taskRecordDivisionID, const std::string &taskStatus)
{
  if(taskStatus.compare("open") == 0 && CMS_RIGHTS->checkTask(taskID, df::rights::rtCanEdit, taskRecordDivisionID, DF_SESSION_MEMBER_ID_INT))
  {
    HTML
    (
      <!++
       <tabpage++ [Добавление] [comments] [comments_add] [tabCommentAdd]>
       <form method='POST' action='/tasks/addcomment/<v++ taskID ++v>'>
         <labeled_control++ [Текст комментария] [commentText]>
           <textarea id="id_comment" name="comment"/></textarea>
         <++labeled_control>
         <input type="submit" value="Отправить" />
       </form>
       <++tabpage>
      ++!>
    );
  }
}
// ------------------------------------ Комментарии ---------------------------------

std::string CTasks::updateChangelog(const int &taskID, const std::string &what, const std::string &description)
{
  CHTMLGenerator::updateChangelog("task", taskID, what, description);
  return description + "<br />";
}



}
}
