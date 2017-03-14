#include "ccalendar.h"
#include "html.h"
#include "cquery.h"

namespace df
{
namespace ui
{

CCalendar::CCalendar(cppcms::service &srv)
  : CUserPage(srv)
{
  dispatcher().assign("month/(\\d+)/(\\d++)"      , &CCalendar::showMonth  , this, 1, 2);
  dispatcher().assign("day/(\\d+)/(\\d++)/(\\d++)", &CCalendar::showDay    , this, 1, 2, 3);
  dispatcher().assign(""                          , &CCalendar::defaultPage, this);
}

CCalendar::~CCalendar()
{}

void CCalendar::composePageHead()
{
  HTML(<!++ <title>Календарь</title> ++!>);
}

void CCalendar::defaultPage()
{
  putBaseHeader();
  std::tm ts  = currentTimestamp();
  showMonth(ts.tm_year + 1900, ts.tm_mon + 1);
  putBaseFooter();
}

void CCalendar::showMonth(std::string year, std::string month)
{
  putBaseHeader();
  showMonth(atoi(year.c_str()), atoi(month.c_str()));
  putBaseFooter();
}

void CCalendar::showMonth(const int &year, const int &month)
{
  std::string dateAsID;
  std::tm ts, prevTs, nextTs;
  ts.tm_sec = ts.tm_min = ts.tm_hour = ts.tm_mday = 1;
  ts.tm_mon = month - 1;
  ts.tm_year = year - 1900;
  std::mktime(&ts);
  prevTs = nextTs = ts;
  prevTs.tm_mon--; std::mktime(&prevTs);
  nextTs.tm_mon++; std::mktime(&nextTs);
  HTML
  (
    <!++
      <div class='date_change_control'>
       <img class='icon_button' src='/media/img/icons/arrow-left-b.svg'  onclick='window.open(<dq++/calendar/month/<v++ prevTs.tm_year+1900 ++v>/<v++ prevTs.tm_mon+1 ++v>++dq>, <dq++_self++dq>);' />
       &nbsp;&nbsp;<span class='month_caption'><v++ m_monthNames[ts.tm_mon] ++v>&nbsp;&nbsp;<v++ year ++v></span>&nbsp;&nbsp;
       <img class='icon_button' src='/media/img/icons/arrow-right-b.svg' onclick='window.open(<dq++/calendar/month/<v++ nextTs.tm_year+1900 ++v>/<v++ nextTs.tm_mon+1 ++v>++dq>, <dq++_self++dq>);' />
      </div>
      <table class='calendar'>
      <tr><th>Понедельник</th><th>Вторник</th><th>Среда</th><th>Четверг</th><th>Пятница</th><th>Суббота</th><th>Воскресенье</th></tr>
    ++!>
  );
  int weekday = ts.tm_wday == 0 ? 7 : ts.tm_wday;
  if(weekday > 1)
  {
    HTML(<!++ <tr> ++!>);
    while (weekday > 1)
    {
      weekday--;
      HTML(<!++ <td>&nbsp;</td> ++!>);
    }
  }
  do
  {
    dateAsID = std::to_string(ts.tm_year + 1900) + std::to_string(ts.tm_mon + 1) + std::to_string(ts.tm_mday);
    weekday = ts.tm_wday == 0 ? 7 : ts.tm_wday;
    if(weekday == 1)
    {
      HTML(<!++ </tr><tr> ++!>);
    }
    HTML
    (
      <!++
        <td class='clickable' onclick="window.open('/calendar/day/<v++ ts.tm_year + 1900 ++v>/<v++ ts.tm_mon + 1 ++v>/<v++ ts.tm_mday ++v>', '_self');">
          <div class='date <v++ (weekday > 5 ? <s++ holyday ++s> : <es++>) ++v>'><v++ ts.tm_mday ++v></div>
          <div class='body'>
            <div id='tasks_stat_<v++ dateAsID ++v>' class='tasks_calendar_stat'></div>
            <thread_load++ ['/tasks/daytotal/<v++ ts.tm_year + 1900++v>/<v++ ts.tm_mon + 1 ++v>/<v++ ts.tm_mday ++v>'] [tasks_stat_<v++ dateAsID ++v>]>
          </div>
        </td>
      ++!>
    );
    ts.tm_mday += 1;
    std::mktime(&ts);
  }
  while (ts.tm_mday > 1);
  ts.tm_mday -= 1;
  std::mktime(&ts);
  weekday = ts.tm_wday == 0 ? 7 : ts.tm_wday;
  if(weekday < 7)
  {
    while (weekday < 7)
    {
      weekday++;
      HTML(<!++ <td>&nbsp;</td> ++!>);
    }
    HTML(<!++ </tr> ++!>);
  }
  HTML(<!++ </table> ++!>);
}

void CCalendar::showDay(std::string year, std::string month, std::string day)
{
  putBaseHeader();
  showDay(atoi(year.c_str()), atoi(month.c_str()), atoi(day.c_str()));
  putBaseFooter();
}

void CCalendar::showDay(const int &year, const int &month, const int &day)
{
  std::tm ts, prevTs, nextTs;
  ts.tm_sec  = ts.tm_min = ts.tm_hour = 1;
  ts.tm_mday = day;
  ts.tm_mon  = month - 1;
  ts.tm_year = year - 1900;
  std::mktime(&ts);
  prevTs = nextTs = ts;
  prevTs.tm_mday--; std::mktime(&prevTs);
  nextTs.tm_mday++; std::mktime(&nextTs);
  int weekday = ts.tm_wday == 0 ? 7 : ts.tm_wday;
  std::string dateAsID = std::to_string(ts.tm_year + 1900) + std::to_string(ts.tm_mon + 1) + std::to_string(ts.tm_mday);
  HTML
  (
    <!++
        <table class='calendar'>
          <tr>
            <td>
              <div class='date <v++ (weekday > 5 ? <s++ holyday ++s> : <es++>) ++v>'>
                <div class='date_change_control'>
                 <img class='icon_button' src='/media/img/icons/arrow-left-b.svg'  onclick='window.open(<dq++/calendar/day/<v++ prevTs.tm_year + 1900 ++v>/<v++ prevTs.tm_mon + 1 ++v>/<v++ prevTs.tm_mday ++v>++dq>, <dq++_self++dq>);' />
                 &nbsp;&nbsp;<span class='month_caption'><v++ ts.tm_mday ++v>.<v++ ts.tm_mon + 1 ++v>.<v++ ts.tm_year + 1900 ++v>, <v++ m_weekNames[weekday-1] ++v>&nbsp;&nbsp;
                 <img class='icon_button' src='/media/img/icons/arrow-right-b.svg' onclick='window.open(<dq++/calendar/day/<v++ nextTs.tm_year + 1900 ++v>/<v++ nextTs.tm_mon + 1 ++v>/<v++ nextTs.tm_mday ++v>++dq>, <dq++_self++dq>);' />
                </div>
              </div>
              <div class='body'>
                <div id='tasks_stat_<v++ dateAsID ++v>' class='tasks_calendar_stat'></div>
                <thread_load++ ['/tasks/daytotal/<v++ ts.tm_year + 1900++v>/<v++ ts.tm_mon + 1 ++v>/<v++ ts.tm_mday ++v>'] [tasks_stat_<v++ dateAsID ++v>]>
              </div>
            </td>
          </tr>
          </table>
          <panel++ [Список задач на данное число] [ ] [visible] [tasklist]>
            <table class='tasklist'>
              <tr><th>В работе<help++ [taskDayInWork]></th><th>Контрольный срок<help++ [taskDayControl]></th></tr>
              <tr>
                <td>
                  <paged_list++ [day_tasks_inwork] [/tasks/list] [taskListRenderer]>
                    'status'   : 'open',
                    'date'     : '<v++ ts.tm_year + 1900++v>-<v++ ts.tm_mon + 1 ++v>-<v++ ts.tm_mday ++v>'
                  <++paged_list>
                </td>
                <td>
                  <paged_list++ [day_tasks_control] [/tasks/list] [taskListRenderer]>
                    'status'   : 'control',
                    'date'     : '<v++ ts.tm_year + 1900++v>-<v++ ts.tm_mon + 1 ++v>-<v++ ts.tm_mday ++v>'
                  <++paged_list>
                </td>
              </tr>
            <tr><th>Запущено в работу<help++ [taskDayStarted]></th><th>Завершено<help++ [taskDayFinished]></th></tr>
            <tr>
              <td>
                <paged_list++ [day_tasks_started] [/tasks/list] [taskListRenderer]>
                  'status'   : 'started',
                  'date'     : '<v++ ts.tm_year + 1900++v>-<v++ ts.tm_mon + 1 ++v>-<v++ ts.tm_mday ++v>'
                <++paged_list>
              </td>
              <td>
                <paged_list++ [day_tasks_finished] [/tasks/list] [taskListRenderer]>
                  'status'   : 'finished',
                  'date'     : '<v++ ts.tm_year + 1900++v>-<v++ ts.tm_mon + 1 ++v>-<v++ ts.tm_mday ++v>'
                <++paged_list>
              </td>
            </tr>
            </table>


          <++panel>

    ++!>
  );
}

}
}
