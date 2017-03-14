#include "chtmlgenerator.h"
#include "cquery.h"
#include "debug.h"
#include "html.h"
#include "cmailer.h"

namespace df
{
namespace ui
{

CHTMLGenerator::CHTMLGenerator(cppcms::service &srv)
  : cppcms::application(srv)
{}

CHTMLGenerator::~CHTMLGenerator()
{}

bool CHTMLGenerator::userIsLoggedOn()
{
//  CMS_DEBUG_LOG("Session member:" << session()["member_id"]);
  return session().is_set(DF_SESSION_MEMBER_ID_KEY) && !DF_SESSION_MEMBER_ID_STR.empty();
}


std::tm CHTMLGenerator::currentTimestamp()
{
  std::time_t now_time = std::time(0);
  return *std::localtime(&now_time);
}

// ---------------------------------------------- division --------------------------------------------------------
void CHTMLGenerator::divisionsSelectBox(const std::string &caption, const std::string &helpID, const int &taskID, const int &currentDivisionID)
{
  std::string selectID = "task_division_id" + (taskID == 0 ? "" : "_" + std::to_string(taskID));
  int divisionID = currentDivisionID ? currentDivisionID : DF_SESSION_MEMBER_DIVISION_ID_INT;
  std::string selected = "";
  DFDB_R divisions = DFDB_Q("divisions_select_box", "select id, short_name from divisions order by short_name", );
  HTML
  (
     <!++
        <labeled_control++ [<v++ caption ++v>] [<v++ helpID ++v>]>
        <select id='<v++ selectID ++v>' name='division_id'>
     ++!>
  );
  DFDB_QLOOP(divisions, division)
  {
    selected = divisionID == division[0].as<int>() ? "selected" : "";
    HTML
    (
       <!++
          <option value='<v++ division[0].as<std::string>() ++v>' <v++ selected ++v>><v++ division[1].as<std::string>() ++v></option>
       ++!>
    );
  }
  HTML(<!++ </select><++labeled_control> ++!>);
}
// ---------------------------------------------- division --------------------------------------------------------
// ---------------------------------------------- member --------------------------------------------------------
void CHTMLGenerator::membersSelectBox(const std::string &caption,
                                      const std::string &type, const std::string &helpID,
                                      const int &uniqueID,
                                      const int &currentMemberID)
{
  membersSelectBox(caption,type,helpID,uniqueID,std::vector<int>() = { currentMemberID });
}
void CHTMLGenerator::membersSelectBox(const std::string &caption,
                      const std::string &type,
                      const std::string &helpID,
                      const int &uniqueID,
                      const std::vector<int> &currentMembersID,
                      const bool &excludeCurrent)
{
  CMS_DEBUG_LOG("!!! 1");
  std::string selectID = "task_" + type + "_id" + (uniqueID == 0 ? "" : "_" + std::to_string(uniqueID));
  std::string where;
  if(type.compare("controller") == 0)
  {
    where += " members.id in (select distinct manager_member_id from divisions) ";
  }
  if(excludeCurrent && currentMembersID.size() && currentMembersID[0] != 0)
  {
    where += std::string(where.empty() ?  " " : " and ") + " members.id not in (";
    for(int m: currentMembersID)
    {
      where += std::to_string(m) + ",";
    }
    where = where.substr(0, where.size()-1);
    where += ") ";
  }
  std::string selected = "";
  DFDB_R members = DFDB_Q( type + "_select_box", DF_SQL_TEXT(
               select
                 members.id,
                 members.name,
                 divisions.short_name
               from members
               left join divisions on divisions.id=members.division_id
               where members.fired=false )
               + (where.empty() ? " " : " and ( " + where + " ) ") +
               DF_SQL_TEXT( order by divisions.short_name, members.name), /* empty */);
  HTML
  (
     <!++
        <labeled_control++ [<v++ caption ++v>] [<v++ helpID ++v>]>
        <select id="<v++ selectID ++v>" name="<v++ type ++v>_id">
     ++!>
  );
  CMS_DEBUG_LOG("!!! 2");
  DFDB_QLOOP(members, member)
  {
    selected = (excludeCurrent ? "" : (currentMembersID[0] == member[0].as<int>() ? "selected" : ""));
    HTML
    (
       <!++
          <option value='<v++ member[0].as<std::string>() ++v>' <v++ selected ++v>><v++ member[2].as<std::string>() ++v> :: <v++ member[1].as<std::string>() ++v></option>
       ++!>
    );
  }
  HTML(<!++ </select><++labeled_control> ++!>);
  CMS_DEBUG_LOG("!!! 3");
}

// ---------------------------------------------- member --------------------------------------------------------
// ---------------------------------------------- date --------------------------------------------------------
void CHTMLGenerator::dateEdit(const std::string &caption, const std::string &name, std::string currentDate, const int &id)
{
  if(currentDate.empty())
  {
    std::tm ts  = currentTimestamp();
    currentDate = std::to_string(ts.tm_mday) + "." + std::to_string(ts.tm_mon+1) + "." + std::to_string(static_cast<int>(ts.tm_year) + 1900);
  }
  std::string editID = name + (id == 0 ? "" : "_" + std::to_string(id));
  HTML
  (
     <!++
        <labeled_control++ [<v++ caption ++v>]>
        <input type="text" id='<v++ editID ++v>' name="<v++ name ++v>" value="<v++ currentDate ++v>"/>
        <++labeled_control>
        <script>
           makeJQUIDateEdit("<v++ editID ++v>");
        </script>
     ++!>
  );
}

void CHTMLGenerator::dateTimeEdit(const std::string &caption, const std::string &name, const std::string &helpID, std::string currentDateTime, const int &id)
{
  if(currentDateTime.empty())
  {
    std::tm ts  = currentTimestamp();
    currentDateTime = std::to_string(ts.tm_mday) + "." + std::to_string(ts.tm_mon+1) + "." + std::to_string(static_cast<int>(ts.tm_year) + 1900)
                                                 + " " + std::to_string(ts.tm_hour)  + ":" + std::to_string(ts.tm_min) + ":" + std::to_string(ts.tm_sec);
  }
  std::string editID = name + (id == 0 ? "" : "_" + std::to_string(id));
  HTML
  (
     <!++
        <labeled_control++ [<v++ caption ++v>] [<v++ helpID ++v>]>
        <input type="text" id='<v++ editID ++v>' name="<v++ name ++v>" value="<v++ currentDateTime ++v>"/>
        <++labeled_control>
        <script>
           makeJQUIDateTimeEdit("<v++ editID ++v>");
        </script>
     ++!>
  );
}
// ---------------------------------------------- date --------------------------------------------------------

void CHTMLGenerator::updateChangelog(const std::string &chClass, const int &classID,
                                     const std::string &what, const std::string &description)
{
  DFDB_Q("update_changelog", "insert into changelog ("
                                "class_of_change,"
                                "class_id,"
                                "changer_member_id,"
                                "what_changed,"
                                "description) values ($1, $2, $3, $4, $5)",
                      (chClass)
                      (classID)
                      (DF_SESSION_MEMBER_ID_INT)
                      (what)
  (description));
  DF_MAILER->send(chClass, classID, what, description);
}

#define DF_POST_PREPARE \
  cppcms::http::request::form_type post = request().post(); \
  cppcms::http::request::form_type::iterator it = post.find(key);
#define DF_POST_KEY_EXISTS it != post.end()
#define DF_POST_VALUE (it->second)

std::string CHTMLGenerator::postValue(const std::string &key, const std::string &deflt)
{
  DF_POST_PREPARE;
  return DF_POST_KEY_EXISTS ? DF_POST_VALUE : deflt;
}

int CHTMLGenerator::postValue(const std::string &key, const int &deflt)
{
  DF_POST_PREPARE;
  return DF_POST_KEY_EXISTS ? atoi(DF_POST_VALUE.c_str()) : deflt;
}

bool CHTMLGenerator::postValueExists(const std::string &key)
{
  DF_POST_PREPARE;
  return DF_POST_KEY_EXISTS;
}

bool CHTMLGenerator::postValueIs(const std::string &key, const std::string &compare)
{
  DF_POST_PREPARE;
  if(DF_POST_KEY_EXISTS)
  {
    return DF_POST_VALUE.compare(compare) == 0;
  }
  return false;
}

bool CHTMLGenerator::postValueIsEmpty(const std::string &key)
{
  DF_POST_PREPARE;
  if(DF_POST_KEY_EXISTS)
  {
    return DF_POST_VALUE.empty();
  }
  return true;
}


}
}
