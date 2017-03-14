#include "capplication.h"
#include "cauthorize.h"
#include "cdataloader.h"
#include "cimage.h"
#include "cmainpage.h"
#include "cmenu.h"
#include "ctasks.h"
#include "cmembers.h"
#include "cdivisions.h"
#include "chelp.h"
#include "cattachedfiles.h"
#include "ccalendar.h"
#include "cmaintenancework.h"
#include "cmaintenanceworkclass.h"
#include "debug.h"

#include <cppcms/http_request.h>
#include <cppcms/http_context.h>


namespace df
{
namespace model
{

CApplication::CApplication(cppcms::service &srv)
  : cppcms::application(srv)
{
//  attach(new df::model::CDataLoader(srv), "data"           , "/data{1}"          , "/data/?(.*)"          , 1);
  attach(new df::ui::CAuthorize           (srv), "auth"           , "/auth{1}"          , "/auth/?(.*)"          , 1);
  attach(new df::ui::CMenu                (srv), "menu"           , "/menu{1}"          , "/menu/?(.*)"          , 1);
  attach(new df::ui::CTasks               (srv), "tasks"          , "/tasks{1}"         , "/tasks/?(.*)"         , 1);
  attach(new df::ui::CMembers             (srv), "members"        , "/members{1}"       , "/members/?(.*)"       , 1);
  attach(new df::ui::CDivisions           (srv), "divisions"      , "/divisions{1}"     , "/divisions/?(.*)"     , 1);
  attach(new df::ui::CHelp                (srv), "help"           , "/help{1}"          , "/help/?(.*)"          , 1);
  attach(new df::ui::CAttachedFiles       (srv), "file"           , "/file{1}"          , "/file/?(.*)"          , 1);
  attach(new df::ui::CCalendar            (srv), "calendar"       , "/calendar{1}"      , "/calendar/?(.*)"      , 1);
  attach(new df::ui::CMaintenanceWorkClass(srv), "mworkclass"     , "/mworkclass{1}"    , "/mworkclass/?(.*)"    , 1);
  attach(new df::ui::CMaintenanceWork     (srv), "mwork"          , "/mwork{1}"         , "/mwork/?(.*)"         , 1);
  attach(new df::ui::CImage               (srv), "data/media/img" , "/data/media/img{1}", "/data/media/img/?(.*)", 1);
  attach(new df::ui::CMainPage            (srv), "main"           , "/{1}"              , "/?(.*)"               , 1);
}

CApplication::~CApplication()
{}



}
}
