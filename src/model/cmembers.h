#ifndef CMEMBERS_H
#define CMEMBERS_H
#include "cuserpage.h"
namespace df
{
namespace ui
{

class CMembers: public CUserPage
{
  public:
    CMembers(cppcms::service &srv);
    ~CMembers();
  private:
    void defaultPage();
    void membersList();
    void showAcceptedChangelog(std::string memberID);
    void memberAdd();
    void memberDelete();
    void showMember(std::string id);
    void composePageHead();
    void updateChangelog(const int &memberID, const std::string &what, const std::string &description);
    void makeMemberControls(std::string id);
    void memberEditForm(const std::string &action,
                        const std::string &fio = std::string(),
                        const std::string &email = std::string(),
                        const std::string &pw = std::string(),
                        const int &divisionID = 0);
    void makeMemberRightsControl(std::string id);
    void setMemberRights(const std::string &id, const std::string &objectClass, const std::string &rightType, const bool &allow);
};

}
}
#endif // CMEMBERS_H
