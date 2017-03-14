#include "cmaintenanceworkclass.h"
#include "cquery.h"
#include "html.h"
#include "crights.h"

namespace df
{
namespace ui
{

CMaintenanceWorkClass::CMaintenanceWorkClass(cppcms::service &srv)
  : CUserPage(srv)
{
  dispatcher().assign(""    , &CMaintenanceWorkClass::defaultPage, this);
  dispatcher().assign("add" , &CMaintenanceWorkClass::addClass,    this);
  dispatcher().assign("list", &CMaintenanceWorkClass::classesList, this);
}

CMaintenanceWorkClass::~CMaintenanceWorkClass()
{

}

void CMaintenanceWorkClass::composePageHead()
{
  HTML(<!++ <title>Классы регламентных работ</title> ++!>);
}

void CMaintenanceWorkClass::defaultPage()
{
  putBaseHeader();
  HTML
  (
    <!++
      <panel++ [Список классов регламентных работ] [ ] [visible] [mvcclasses]>
        <paged_list++ [maintenance_classes] [/mworkclass/list] [maintenanceWorkClassesListRenderer]>
        <++paged_list>
      <++panel>
    ++!>
  );
  putBaseFooter();
}

void CMaintenanceWorkClass::classesList()
{

}

void CMaintenanceWorkClass::addClass()
{
  if(DF_REQUEST_IS_POST)
  {
    DF_STOP_IF_NO_AUTH;
  }
  else
  {
    putBaseHeader();
    HTML
    (
      <!++
      <panel++ [Добавление класса регламентных работ] [ ] [visible] [taskadd]>
       <form method='POST' action='/mworkclass/add'>
         <table class='form'>
         <tr>
            <td>
               <labeled_control++ [Краткое описание регламентной работы] [maintenanceWorkClassCaption]>
                 <input type="text" id="id_caption" name="caption"/>
               <++labeled_control>
            </td>
            <td>
                  <f++ divisionsSelectBox(<s++Целевое подразделение++s>, <s++maintenanceWorkClassTargetDivision++s>); ++f>
            </td>
            <td>
                  <f++ membersSelectBox(<s++Контроль исполнения за++s>, <s++controller++s>, <s++maintenanceWorkClassController++s>); ++f>
            </td>
         </tr>
         </table>
         <labeled_control++ [Описание регламентной работы] [maintenanceWorkClassDescrition]>
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

}
}
