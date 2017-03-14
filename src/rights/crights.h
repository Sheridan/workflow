#ifndef CRIGHTS_H
#define CRIGHTS_H

#include <string>
#include <map>

namespace df
{
namespace rights
{

enum EObjectsTypes
{
  otTask,
  otMember,
  otDivision
};

enum ERightsTypes
{
  rtAdmin,
  rtCanEdit,
  rtCanAdd,
  rtCanDelete,
  rtCanWorkAsDivisionManager
};

typedef std::map</*member id*/ int, std::map</* division id */ int, std::map<EObjectsTypes, std::map<ERightsTypes, bool>>>> TRightsMap;
typedef std::map</*member id*/ int, bool> TAdmins;
typedef std::map<EObjectsTypes, std::string> TObjectNames;
typedef std::map<ERightsTypes, std::string> TRightsNames;
typedef std::map</* member id */ int, std::map</*division id*/ int, bool>> TDivisionsManagers;
typedef std::map</* member id*/ int, std::map</* task id*/ int, std::map<ERightsTypes, bool>>> TTaskRights;

class CRights
{
  public:
    static CRights * instance();
    void destroy();
    bool check(const EObjectsTypes &obj, const ERightsTypes &requestedRights, const int &objDivisionID, const int &sessionMemberID);
    void clear(const int &sessionMemberID);
    void clearTask(const int &sessionMemberID, const int &taskID);
    bool checkTask(const int &taskID,
                   const ERightsTypes &requestedRights,
                   const int &taskDivisionID,
                   const int &sessionMemberID);
    bool checkMember(const ERightsTypes &requestedRights,
                     const int &sessionMemberID);
    bool checkDivision(const ERightsTypes &requestedRights,
                       const int &sessionMemberID);
  private:
    CRights();
    virtual ~CRights();
    bool isAdmin(const EObjectsTypes &obj, const int &memberID);
    bool check(const EObjectsTypes &obj, const ERightsTypes &requestedRights, const int &memberID); // Проверка на принципиальное наличие прав у пользователя
    bool isDivisionManager(const int &divisionID, const int &memberID);
    // caches
    TAdmins    m_admins;
    TRightsMap m_rightsCache;
    TObjectNames m_objectNames;
    TRightsNames m_rightsNames;
    TDivisionsManagers m_divisionsManagers;
    TTaskRights m_taskRights;
};

}
}

#define CMS_RIGHTS df::rights::CRights::instance()

#endif // CRIGHTS_H
