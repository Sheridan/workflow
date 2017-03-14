#include "crights.h"
#include "cquery.h"
namespace df
{
namespace rights
{

CRights *rights_instance = nullptr;

CRights *CRights::instance()
{
  if(rights_instance) { return rights_instance; }
  return new CRights();
}

void CRights::destroy()
{
  delete rights_instance;
  rights_instance = nullptr;
}

bool CRights::check(const EObjectsTypes &obj, const ERightsTypes &requestedRights, const int &objDivisionID, const int &sessionMemberID)
{
  if(
      m_rightsCache.find(sessionMemberID)                                      == m_rightsCache.end()                                     ||
      m_rightsCache[sessionMemberID].find(objDivisionID)                       == m_rightsCache[sessionMemberID].end()                    ||
      m_rightsCache[sessionMemberID][objDivisionID].find(obj)                  == m_rightsCache[sessionMemberID][objDivisionID].end()     ||
      m_rightsCache[sessionMemberID][objDivisionID][obj].find(requestedRights) == m_rightsCache[sessionMemberID][objDivisionID][obj].end()
    )
  {
    DF_RIGHTS_DEBUG("Checking rights (" << m_rightsNames[requestedRights] << ") for member " << sessionMemberID <<
                    " for object " << m_objectNames[obj] <<
                    " (with id: " << objDivisionID << ") ");
    bool result = isAdmin(obj, sessionMemberID);
    if(!result) // Не админ, проверяем дальше
    {
      result = check(obj, requestedRights, sessionMemberID);
      if(result && requestedRights != rtCanWorkAsDivisionManager) // Принципиально запрашиваемые права есть, надо проверить принадлежность к подразделению
                                                         // если не запрашиваются права работы как руководителя подразделения
      {
        DFDB_R memberDivision = DFDB_Q("member_division_id", "select division_id from members where id=$1",(sessionMemberID));
        int memberDivisionID = memberDivision[0][0].as<int>();
        result = (memberDivisionID == objDivisionID);
        if(!result) // Пользователь не принадлежит подразделению, может быть он начальник вышестоящего?
        {
          result = isDivisionManager(objDivisionID, sessionMemberID);
          if(!result) // Не начальник. Может быть есть права работы как начальника?
          {
            DFDB_R divisionManager = DFDB_Q("division_manager_of_member_division",
                                           "select divisions.manager_member_id "
                                           "from divisions "
                                           "where divisions.id = "
                                             "("
                                               "select members.division_id "
                                               "from members "
                                               "where members.id=$1 "
                                             ")",(objDivisionID));
            result = check(obj, requestedRights, objDivisionID, divisionManager[0][0].as<int>()) && check(obj, rtCanWorkAsDivisionManager, objDivisionID, sessionMemberID);
          }
        }
      }
    }
    m_rightsCache[sessionMemberID][objDivisionID][obj][requestedRights] = result;
  }
  DF_RIGHTS_DEBUG("Rights (" << m_rightsNames[requestedRights] << ") for member " << sessionMemberID <<
                  " for object " << m_objectNames[obj] <<
                  " (with id: " << objDivisionID << "): " << m_rightsCache[sessionMemberID][objDivisionID][obj][requestedRights]);
  return m_rightsCache[sessionMemberID][objDivisionID][obj][requestedRights];
}

bool CRights::check(const EObjectsTypes &obj, const ERightsTypes &requestedRights, const int &memberID)
{
  return DFDB_Q("check_allow",
                "select "
                  "allow "
                "from members_rights "
                "where member_id=$1 and allow=$2 and object_class=$3",
                (memberID)
                (m_rightsNames[requestedRights])
                (m_objectNames[obj])).size() > 0;
}

bool CRights::isDivisionManager(const int &divisionID, const int &memberID)
{

  if(
      m_divisionsManagers.find(memberID)             == m_divisionsManagers.end()           ||
      m_divisionsManagers[memberID].find(divisionID) == m_divisionsManagers[memberID].end()
    )
  {
    bool result = false;
    DFDB_R managers = DFDB_Q("all_managers", "select manager_member_id from managed_divisions($1)",(memberID));
    DFDB_QLOOP(managers, manager)
    {
      if(memberID == manager[0].as<int>())
      {
        result = true;
        break;
      }
    }
    m_divisionsManagers[memberID][divisionID] = result;
  }
  return m_divisionsManagers[memberID][divisionID];
}

void CRights::clear(const int &sessionMemberID)
{
  m_rightsCache[sessionMemberID].clear();
  m_divisionsManagers[sessionMemberID].clear();
  m_admins.erase(sessionMemberID);
  m_taskRights[sessionMemberID].clear();
  DF_RIGHTS_DEBUG("Rights for member " << sessionMemberID << " cleared");
}

void CRights::clearTask(const int &sessionMemberID, const int &taskID)
{
  m_taskRights[sessionMemberID][taskID].clear();
  DF_RIGHTS_DEBUG("Rights for member " << sessionMemberID << " for task " << taskID << " cleared");
}

CRights::CRights()
{
  m_objectNames =
  {
    { otTask    , "task"     },
    { otMember  , "member"   },
    { otDivision, "division" }
  };
  m_rightsNames =
  {
    { rtAdmin                   , "admin"                        },
    { rtCanEdit                 , "can_edit"                     },
    { rtCanAdd                  , "can_add"                      },
    { rtCanDelete               , "can_delete"                   },
    { rtCanWorkAsDivisionManager, "can_work_as_division_manager" }
  };
  rights_instance = this;
}

CRights::~CRights()
{

}

bool CRights::isAdmin(const EObjectsTypes &obj, const /*member id*/ int &memberID)
{
  if(m_admins.find(memberID) == m_admins.end())
  {
    m_admins[memberID] = check(obj, rtAdmin, memberID);
  }
  return m_admins[memberID];
}

bool CRights::checkTask(const int &taskID,
                        const ERightsTypes &requestedRights,
                        const int &taskDivisionID,
                        const int &sessionMemberID)
{
  /*
   * Задачу изменить можно если
   * 1. Я - создатель задачи
   * 2. Я - руководитель отдела, которому поставлена задача
   * 3. Я - контролирующий выполнение задачи
   * 4. Я - руководитель отдела, оторому подчиняется отдел, которому ..., ..., которому поставлена задача
   */
  if(
      m_taskRights.find(sessionMemberID)                          == m_taskRights.end()                         ||
      m_taskRights[sessionMemberID].find(taskID)                  == m_taskRights[sessionMemberID].end()        ||
      m_taskRights[sessionMemberID][taskID].find(requestedRights) == m_taskRights[sessionMemberID][taskID].end()
    )
  {
    DF_RIGHTS_DEBUG("Checking task rights (" << m_rightsNames[requestedRights] << ") for member " << sessionMemberID <<
                    " for task " << taskID <<
                    " (task division: " <<
                    taskDivisionID << ") ");
    DFDB_R taskInfo = DFDB_Q("task_members_and_status",
                            "select "
                              "performer_member_id, "
                              "creator_member_id, "
                              "controller_member_id, "
                              "status='closed' "
                            "from tasks "
                            "where id=$1",
                            (taskID));
    m_taskRights[sessionMemberID][taskID][requestedRights] =
                  (
                    !(taskInfo[0][3].as<bool>() && requestedRights == rtCanEdit) || // Задача закрыта, а запрашивается редактирование - запретить.
                     (taskInfo[0][3].as<bool>() && requestedRights == rtCanAdd)     // Но если запрашивается добавление (для открытия заново) - разрешить
                  )
                  &&
                  (
                    (sessionMemberID == taskInfo[0][1].as<int>(0)) || // пользователь - владелец
                    (sessionMemberID == taskInfo[0][2].as<int>(0)) || // пользователь - контроллер
                    check(otTask, requestedRights, taskDivisionID, sessionMemberID)
                  );
  }
  DF_RIGHTS_DEBUG("Task rights (" << m_rightsNames[requestedRights] << ") for member " << sessionMemberID <<
                  " for task " << taskID <<
                  " (task division: " << taskDivisionID << ")" <<
                  ": " << m_taskRights[sessionMemberID][taskID][requestedRights]);
  return m_taskRights[sessionMemberID][taskID][requestedRights];
}

bool CRights::checkMember(const ERightsTypes &requestedRights, const int &sessionMemberID)
{
  /*
   * Пользователя изменить можно если
   * 1. Я - админ
   */
  switch (requestedRights)
  {
    case rtAdmin:
    case rtCanAdd:
    case rtCanEdit:
    case rtCanDelete:
    {
      return isAdmin(otMember, sessionMemberID);
    } break;
    case rtCanWorkAsDivisionManager: { return false; } break;
  }
}

bool CRights::checkDivision(const ERightsTypes &requestedRights, const int &sessionMemberID)
{
  /*
   * Подразделение изменить можно если
   * 1. Я - админ
   */
  switch (requestedRights)
  {
    case rtAdmin:
    case rtCanAdd:
    case rtCanEdit:
    case rtCanDelete:
    {
      return isAdmin(otMember, sessionMemberID);
    } break;
    case rtCanWorkAsDivisionManager: { return false; } break;
  }
}

}
}
