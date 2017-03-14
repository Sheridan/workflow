#ifndef CHTMLGENERATOR_H
#define CHTMLGENERATOR_H
#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/session_interface.h>
#include <ctime>
#include <vector>

namespace df
{
namespace ui
{

class CHTMLGenerator : public cppcms::application
{
  public:
    CHTMLGenerator(cppcms::service &srv);
    ~CHTMLGenerator();

  protected:
    bool userIsLoggedOn();
    std::tm currentTimestamp();
    void divisionsSelectBox(const std::string &caption, const std::string &helpID, const int &taskID = 0, const int &currentDivisionID = 0);
    void membersSelectBox(const std::string &caption,
                          const std::string &type,
                          const std::string &helpID,
                          const int &uniqueID = 0,
                          const int &currentMemberID = 0);
    void membersSelectBox(const std::string &caption,
                          const std::string &type,
                          const std::string &helpID,
                          const int &uniqueID,
                          const std::vector<int> &currentMembersID,
                          const bool &excludeCurrent = false);
    void dateEdit(const std::string &caption, const std::string &name, std::string currentDate = "", const int &id = 0);
    void dateTimeEdit(const std::string &caption, const std::string &name, const std::string &helpID, std::string currentDateTime = "", const int &id = 0);
    void updateChangelog(const std::string &chClass, const int &classID,
                         const std::string &what, const std::string &description);

    std::string postValue(const std::string &key, const std::string &deflt = std::string());
    int postValue(const std::string &key, const int &deflt);
    bool postValueExists(const std::string &key);
    bool postValueIs(const std::string &key, const std::string &compare);
    bool postValueIsEmpty(const std::string &key);

};

#define DF_REQUEST_IS_POST request().request_method().compare("POST") == 0
#define DF_SESSION_MEMBER_ID_KEY "member_id"
#define DF_SESSION_MEMBER_ID_STR this->session().get<std::string>(DF_SESSION_MEMBER_ID_KEY)
#define DF_SESSION_MEMBER_ID_INT this->session().get<int>(DF_SESSION_MEMBER_ID_KEY)

#define DF_SESSION_MEMBER_DIVISION_ID DFDB_Q("member_division_id", "select division_id from members where id=$1", (DF_SESSION_MEMBER_ID_INT))
#define DF_SESSION_MEMBER_DIVISION_ID_STR DF_SESSION_MEMBER_DIVISION_ID[0][0].as<std::string>()
#define DF_SESSION_MEMBER_DIVISION_ID_INT DF_SESSION_MEMBER_DIVISION_ID[0][0].as<int>()

#define DF_SESSION_MEMBER_DIVISION_MANAGER_ID DFDB_Q("member_division_manager_id", "select manager_member_id from divisions where id=(select division_id from members where id=$1)", (DF_SESSION_MEMBER_ID_INT))
#define DF_SESSION_MEMBER_DIVISION_MANAGER_ID_STR DF_SESSION_MEMBER_DIVISION_MANAGER_ID[0][0].as<std::string>()
#define DF_SESSION_MEMBER_DIVISION_MANAGER_ID_INT DF_SESSION_MEMBER_DIVISION_MANAGER_ID[0][0].as<int>()

}
}
#endif // CHTMLGENERATOR_H
