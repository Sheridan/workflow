#include "cmaintenancework.h"
#include "cquery.h"
#include "html.h"
#include "crights.h"

namespace df
{
namespace ui
{

CMaintenanceWork::CMaintenanceWork(cppcms::service &srv)
: CUserPage(srv)
{
  dispatcher().assign(""                         , &CMaintenanceWork::defaultPage            , this      );
}

CMaintenanceWork::~CMaintenanceWork() {}

void CMaintenanceWork::composePageHead()
{
  HTML(<!++ <title>Регламентные работы</title> ++!>);
}

void CMaintenanceWork::defaultPage()
{

}

}
}
