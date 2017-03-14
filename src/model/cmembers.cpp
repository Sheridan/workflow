#include "cmembers.h"
#include "cquery.h"
#include "html.h"
#include "crights.h"

namespace df
{
namespace ui
{

CMembers::CMembers(cppcms::service &srv)
 : CUserPage(srv)
{
  dispatcher().assign(""                         , &CMembers::defaultPage            , this      );
  dispatcher().assign("list"                     , &CMembers::membersList            , this      );
  dispatcher().assign("add"                      , &CMembers::memberAdd              , this      );
  dispatcher().assign("delete"                   , &CMembers::memberDelete           , this      );
  dispatcher().assign("show/(\\d+)"              , &CMembers::showMember             , this, 1   );
  dispatcher().assign("edit/(\\d+)"              , &CMembers::makeMemberControls     , this, 1   );
  dispatcher().assign("changelog/(\\d+)/accepted", &CMembers::showAcceptedChangelog  , this, 1   );
  dispatcher().assign("setrights/(\\d+)"         , &CMembers::makeMemberRightsControl, this, 1   );
}

CMembers::~CMembers()
{}

void CMembers::composePageHead()
{
  HTML(<!++ <title>Пользователи</title> ++!>);
}

void CMembers::defaultPage()
{
  putBaseHeader();
  HTML
  (
    <!++
      <panel++ [Список пользователей] [ ] [visible] [pmemberslist]>
        <div id='members_list'></div>
      <thread_load++ ['/members/list'] [members_list]>
      <++panel>
    ++!>
  );
  putBaseFooter();
}

void CMembers::membersList()
{
  DFDB_R members = DFDB_Q("members_list", DF_SQL_TEXT(select
   /* 0*/ members.id,
   /* 1*/ members.name,
   /* 2*/ divisions.id,
   /* 3*/ divisions.short_name,
   /* 4*/ to_char(members.last_seen_timestamp, 'DD.MM.YYYY HH24:MI'),
   /* 5*/ (select count(tasks_creator.id)    from tasks tasks_creator    where tasks_creator.creator_member_id       = members.id and tasks_creator.status    in ('open', 'not_assigned')),
   /* 6*/ (select count(tasks_controller.id) from tasks tasks_controller where tasks_controller.controller_member_id = members.id and tasks_controller.status in ('open', 'not_assigned')),
   /* 7*/ (select count(tasks_performer.id)  from tasks tasks_performer  where tasks_performer.performer_member_id   = members.id and tasks_performer.status  in ('open', 'not_assigned')),
   /* 8*/ (select count(changelog.id) from changelog where changelog.changer_member_id = members.id)
        from members
        left join divisions on divisions.id = members.division_id
        where members.id>0 and members.fired=false
        order by divisions.short_name, members.name ), /* empty */);
  HTML
  (
    <!++
      <table class="members_list">
      <tr>
        <th>ФИО</th>
        <th>Отдел</th>
        <th>Крайний вход</th>
        <th>Задач создано</th>
        <th>Задач на контроле</th>
        <th>Задач на исполнении</th>
        <th>Внесено правок</th>
      </tr>
    ++!>
  );
  DFDB_QLOOP(members, member)
  {
    HTML
    (
      <!++
          <tr>
            <td><a href='/members/show/<v++ member[0].as<std::string>() ++v>'><v++ member[1].as<std::string>() ++v></a></td>
            <td><a href='/divisions/show/<v++ member[2].as<std::string>() ++v>'><v++ member[3].as<std::string>() ++v></a></td>
            <td class='datetime'><v++ member[4].as<std::string>() ++v></td>
            <td><v++ member[5].as<std::string>() ++v></td>
            <td><v++ member[6].as<std::string>() ++v></td>
            <td><v++ member[7].as<std::string>() ++v></td>
            <td><v++ member[8].as<std::string>() ++v></td>
          </tr>
      ++!>
    );
  }
  HTML(<!++ </table> ++!>);
}

void CMembers::memberAdd()
{
  if(DF_REQUEST_IS_POST)
  {
    DF_STOP_IF_NO_AUTH
    DFDB_R newmember = DFDB_Q("new_member", "insert into members ("
                                "name, "
                                "password, "
                                "email, "
                                "division_id"
                              ") values ($1, $2, $3, $4) returning id",
                               (postValue("fio"))
                               (postValue("pw"))
                               (postValue("email"))
                               (postValue("division_id")));
    int memberID = CMS_SQL_LAST_INSERT_ID(newmember);
    DFDB_R creator = DFDB_Q("member_name", "select name from members where id=$1", (DF_SESSION_MEMBER_ID_INT));
    updateChangelog(memberID, "member_created",
                    "Зарегестрирован пользователь <a href='/members/show/" + std::to_string(memberID) + "'>" + postValue("fio") + "</a>");
    setMemberRights(std::to_string(memberID), "task", "can_add", true);
    response().set_redirect_header("/members/", cppcms::http::response::see_other);
  }
  else
  {
    putBaseHeader();
    HTML
    (
      <!++
      <panel++ [Добавление пользователя] [ ] [visible] [useradd]>
       <f++ memberEditForm(<s++/members/add++s>); ++f>
      <++panel>
      ++!>
    );
    putBaseFooter();
  }
}

void CMembers::memberDelete()
{
  if(DF_REQUEST_IS_POST)
  {
    DF_STOP_IF_NO_AUTH
    std::string firedMemberID = postValue("fired_member_id");
    bool isFired = DFDB_Q("member_is_fired", "select fired from members where id=$1", (firedMemberID))[0][0].as<bool>();
    DFDB_Q("fire_member",
                  "update members set "
                    "fired=$1 "
                  "where id=$2",
                   (!isFired)
                   (firedMemberID));
    updateChangelog(atoi(firedMemberID.c_str()), "member_edited", "Пользователь <a href='/members/show/" + firedMemberID + "'>" + postValue("fio") + "</a> " + (isFired ? " восстановлен на работе" : " уволен"));
    response().set_redirect_header("/members/show/"+firedMemberID, cppcms::http::response::see_other);
  }
}

void CMembers::showMember(std::string id)
{
  putBaseHeader();
  DFDB_R member = DFDB_Q("member_show",
        "select "
   /* 0*/ "members.id, "
   /* 1*/ "members.name, "
   /* 2*/ "divisions.id, "
   /* 3*/ "divisions.name, "
   /* 4*/ "to_char(members.last_seen_timestamp, 'DD.MM.YYYY HH24:MI'), "
   /* 5*/ "members.fired "
        "from members "
        "left join divisions on divisions.id = members.division_id "
        "where members.id=$1 "
        "order by members.name "
        , (id));
  if(member.size())
  {
    HTML
    (
      <!++
        <panel++ [Основное] [ ] [visible] [member_main]>
        <table class='member_show'>
        <tr>
          <td class='fio'>
            <a href='/members/show/<v++ member[0][0].as<std::string>() ++v>'><v++ member[0][1].as<std::string>() ++v></a> :: <a href='/divisions/show/<v++ member[0][2].as<std::string>() ++v>'><v++ member[0][3].as<std::string>() ++v>
            <v++ (member[0][5].as<bool>() ? <s++<br><b>Уволен</b>++s> : <es++>) ++v>
          </td>
        </tr>
        <tr>
          <th>Крайний вход</th>
        </tr>
        <tr>
          <td>
            <v++ member[0][4].as<std::string>() ++v>
          </td>
        </tr>
        </table>
        <++panel>
        <panel++ [Список отслеживаемых задач] [ ] [visible] [tasklist]>
        <table class='tasklist'>
          <tr><th>В работе</th><th>Исполнено</th></tr>
          <tr>
            <td>
              <paged_list++ [tasks_open] [/tasks/list] [taskListRenderer]>
                'status'   : 'open',
                'member_id': <v++ id ++v>
              <++paged_list>
            </td>
            <td>
              <paged_list++ [tasks_closed] [/tasks/list] [taskListRenderer]>
                'status'   : 'closed',
                'member_id': <v++ id ++v>
              <++paged_list>
            </td>
          </tr>
        </table>
        <++panel>
        <tabcontrol++ [info]>
          <tabpage++ [История] [info] [info_history] [tabHistory]>
            <paged_list++ [member_accepted_changelog] [/members/changelog/<v++ id ++v>/accepted] [memberChangelogRenderer]> <++paged_list>
          <++tabpage>
          <f++ makeMemberControls(id); ++f>
          <f++ makeMemberRightsControl(id); ++f>
        <++tabcontrol>
      ++!>
    );
  }
  putBaseFooter();
}

void CMembers::showAcceptedChangelog(std::string memberID)
{
  DF_STOP_IF_NO_AUTH
  if(DF_REQUEST_IS_POST)
  {
    int firstRecord  = postValue("offset", 0);
    int itemsPerPage = postValue("itemsPerPage", 15);
    DFDB_R acceptedChangelog = DFDB_Q("accepted_changelog",
                "select "
                 /* 0*/ "changelog.id,"
                 /* 1*/ "to_char(changelog.change_timestamp, 'DD.MM.YYYY HH24:MI'),"
                 /* 2*/ "changelog.what_changed,"
                 /* 3*/ "changelog.description,"
                 /* 4*/ "changelog.class_of_change,"
                 /* 5*/ "changelog.class_id, "
                        "case changelog.class_of_change "
                            "when 'task' then (select tasks.caption from tasks where tasks.id=changelog.class_id) "
                            "when 'member' then (select members.name from members where members.id=changelog.class_id) "
                            "else 'Edit query please (CMembers::showMember)' "
                 /* 6*/ "end, "
                 /* 7*/ "changelog_type_to_name(changelog.what_changed) "
                "from changelog "
                "where "
                  "changelog.changer_member_id=$1 "
                "order by changelog.change_timestamp desc "
                "limit $2 offset $3",
                (memberID)(itemsPerPage)(firstRecord));
    cppcms::json::value data;
    int row = 0;
    DFDB_QLOOP(acceptedChangelog, entry)
    {
      data["data"][row][ 0] = entry[ 0].as<int>();
      data["data"][row][ 1] = entry[ 1].as<std::string>("");
      data["data"][row][ 2] = entry[ 2].as<std::string>("");
      data["data"][row][ 3] = entry[ 3].as<std::string>("");
      data["data"][row][ 4] = entry[ 4].as<std::string>("");
      data["data"][row][ 5] = entry[ 5].as<int>();
      data["data"][row][ 6] = entry[ 6].as<std::string>("");
      data["data"][row][ 7] = entry[ 7].as<std::string>("");
      row++;
    }
    DFDB_R count = DFDB_Q("accepted_changelog_count",
                          "select count(changelog.changer_member_id) "
                          "from changelog "
                          "where "
                            "changelog.changer_member_id=$1 ",
                          (memberID));
    data["totalRows"] = count[0][0].as<int>(0);
    response().out() << data;
  }
}

void CMembers::makeMemberControls(std::string id)
{
  if(CMS_RIGHTS->checkMember(df::rights::rtCanAdd, DF_SESSION_MEMBER_ID_INT))
  {
    if(DF_REQUEST_IS_POST)
    {
      DF_STOP_IF_NO_AUTH
      DFDB_Q("edit_member",
                    "update members set "
                      "name=$1, "
                      "password=$2, "
                      "email=$3, "
                      "division_id=$4 "
                    "where id=$5",
                     (postValue("fio"))
                     (postValue("pw"))
                     (postValue("email"))
                     (postValue("division_id"))
                     (id));
      updateChangelog(atoi(id.c_str()), "member_edited", "Пользователь "
                      "<a href='/members/show/" + id + "'>" + postValue("fio") + "</a> изменен");
      response().set_redirect_header("/members/show/" + id, cppcms::http::response::see_other);
    }
    else
    {
      DFDB_R member = DFDB_Q("member_show_edit",
            "select "
       /* 0*/ "members.id, "
       /* 1*/ "members.name, "
       /* 2*/ "members.password, "
       /* 3*/ "members.email, "
       /* 4*/ "members.division_id "
            "from members "
            "where members.id=$1 "
            , (id));
      HTML
      (
        <!++
          <tabpage++ [Редактирование пользователя] [info] [info_member_edit] [tabControl]>
          <f++ memberEditForm(<s++/members/edit/++s> + member[0][0].as<std::string>(), member[0][1].as<std::string>(), member[0][3].as<std::string>(), member[0][2].as<std::string>(), member[0][4].as<int>() ); ++f>
          <panel++ [Удаление пользователя] [ ] [unvisible] [memberdelete]>
            <form method='POST' action='/members/delete'>
            <input type="hidden" name='fired_member_id' value='<v++ member[0][0].as<std::string>() ++v>' />
            <input type="hidden" name='fio' value='<v++ member[0][1].as<std::string>() ++v>' />
            <input type="submit" value='Удалить пользователя сразу, бесплатно и без СМС' />
            </form>
          <++panel>
          <++tabpage>
        ++!>
      );
    }
  }
}

void CMembers::memberEditForm(const std::string &action, const std::string &fio, const std::string &email, const std::string &pw, const int &divisionID)
{
  HTML
  (
    <!++
     <form method='POST' action='<v++ action ++v>'>
       <table class='form'>
       <tr>
         <td>
            <labeled_control++ [Фамилия И. О.] [memberFio]>
            <input type="text" id="id_fio" name="fio" value='<v++ fio ++v>'/>
            <++labeled_control>
         </td>
         <td>
            <labeled_control++ [Пароль] [memberPassword]>
            <input type="text" id="id_pw" name="pw" value='<v++ pw ++v>'/>
            <++labeled_control>
         </td>
         <td>
            <labeled_control++ [E-Mail] [memberEMail]>
            <input type="text" id="id_email" name="email" value='<v++ email ++v>'/>
            <++labeled_control>
         </td>
         <td><f++ divisionsSelectBox(<s++Подразделение++s>, <s++memberDivision++s>, 0, divisionID); ++f></td>
       </tr>
       </table>
       <input type="submit" value="Отправить" />
     </form>
    ++!>
  );
}

void CMembers::makeMemberRightsControl(std::string id)
{
  if(CMS_RIGHTS->checkMember(df::rights::rtCanEdit, DF_SESSION_MEMBER_ID_INT))
  {
    if(DF_REQUEST_IS_POST)
    {
      DF_STOP_IF_NO_AUTH;
      setMemberRights(id, postValue("object_class"), postValue("right_type"), postValueIs("allow", "true"));
    }
    else
    {
      HTML
      (
         <!++
           <tabpage++ [Редактирование прав] [info] [info_member_rights]>
           <table class='member_rights'>
         ++!>
      );
      DFDB_R memberRights = DFDB_Q("member_show_rights",
            "select "
               "coalesce(( "
                 "select true "
                 "from members_rights "
                 "where "
                   "members_rights.member_id=$1 and "
                   "members_rights.allow=rt.right_type and "
                   "members_rights.object_class=oc.obj_class "
                 "limit 1 "
        /* 0*/ "), false) as allow, "
        /* 1*/ "oc.obj_class, "
        /* 2*/ "objects_classes_to_name(oc.obj_class) as obj_class_name, "
        /* 3*/ "rt.right_type, "
        /* 4*/ "rights_types_to_name(rt.right_type) as right_name "
             "from "
               "( "
                 "select unnest(enum_range(NULL::objects_classes)) as obj_class "
               ") oc, "
               "( "
                 "select unnest(enum_range(NULL::rights_types)) as right_type "
               ") rt "
             "order by obj_class, right_type "
            , (id));
      std::string currentObj = "";
      DFDB_QLOOP(memberRights, memberRight)
      {
        if(currentObj.compare(memberRight[1].as<std::string>()) != 0)
        {
          if(!currentObj.empty())
          {
            HTML(<!++ </tr> ++!>);
          }
          HTML(<!++ <tr><th><v++ memberRight[2].as<std::string>() ++v></th> ++!>);
          currentObj = memberRight[1].as<std::string>();
        }
        HTML
        (
           <!++
                <td>
                <input type="checkbox"
                       id="or_<v++ memberRight[1].as<std::string>() ++v>_<v++ memberRight[3].as<std::string>() ++v>"
                       onclick='setMemberRight(<v++ id ++v>, "<v++ memberRight[1].as<std::string>() ++v>", "<v++ memberRight[3].as<std::string>() ++v>");'
                       <v++ (memberRight[0].as<bool>() ? <s++checked++s> : <es++>) ++v> />
                <v++ memberRight[4].as<std::string>() ++v>
                </td>
           ++!>
        );
      }
      HTML(<!++ </table><++tabpage> ++!>);
    }
  }
}

void CMembers::setMemberRights(const std::string &id, const std::string &objectClass, const std::string &rightType, const bool &allow)
{
  if(allow)
  {
    DFDB_Q("set_member_right",
                  "insert into members_rights (member_id, allow, object_class) "
                    "values($1, $2, $3)",
                   (id)
                   (rightType)
                   (objectClass));
  }
  else
  {
    DFDB_Q("unset_member_right",
                  "delete from members_rights where member_id=$1 and allow=$2 and object_class=$3 ",
                   (id)
                   (rightType)
                   (objectClass));
  }
  DFDB_R rightsDescription = DFDB_Q("rights_description",
        "select "
   /* 0*/ "objects_classes_to_name($1), "
   /* 1*/ "rights_types_to_name($2),"
   /* 2*/ "(select name from members where id=$3 limit 1)"
        , (objectClass)
          (rightType)
          (id));
  updateChangelog(atoi(id.c_str()), "member_right_changed", "Пользователю "
                  "<a href='/members/show/" + id + "'>" + rightsDescription[0][2].as<std::string>() + "</a> изменены права для "
                  "[" + rightsDescription[0][0].as<std::string>() + ":" + rightsDescription[0][1].as<std::string>() + "]. "
                  "Новое значение: " + (allow ? "разрешить" : "запретить"));
  CMS_RIGHTS->clear(atoi(id.c_str()));
}

void CMembers::updateChangelog(const int &memberID, const std::string &what, const std::string &description)
{
  CHTMLGenerator::updateChangelog("member", memberID, what, description);
}

}
}
