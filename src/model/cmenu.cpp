#include "cmenu.h"
#include "html.h"
#include "cquery.h"
#include "crights.h"

namespace df
{
namespace ui
{

CMenu::CMenu(cppcms::service &srv)
  : CPagePart(srv)
{
  dispatcher().assign("/?", &CMenu::menu, this);
}

CMenu::~CMenu()
{}

#define DF_MENUITEM(_url, _caption) "<li><a href='" _url "'>" _caption "</a></li>"
#define DF_MENUITEM_NEWPAGE(_url, _caption) "<li><a href='" _url "' target='_blank'>" _caption "</a></li>"
#define DF_SIMPLE_MENUITEM(_url, _caption, _condition) ((_condition) ? DF_MENUITEM(_url, _caption) : "")
#define DF_SIMPLE_MENUITEM_NEWPAGE(_url, _caption, _condition) ((_condition) ? DF_MENUITEM_NEWPAGE(_url, _caption) : "")
#define DF_DIFFERENT_MENUITEM(_url_true, _caption_true, _url_false, _caption_false, _condition) ((_condition) ? DF_MENUITEM(_url_true, _caption_true) : DF_MENUITEM(_url_false, _caption_false))

void CMenu::menu()
{
  bool userIsLogged = userIsLoggedOn();
  bool isITDivision = userIsLogged && (DF_SESSION_MEMBER_DIVISION_ID_INT == 3); // hardcode
  HTML
  (
    <!++
    <ul class='menu'>
      <li><a href='/'>Главная</a></li>
      <li><a href='/calendar'>Календарь</a></li>
      <li><a href='/tasks/'>Задачи</a>
        <ul>
            <v++ DF_SIMPLE_MENUITEM(<s++/tasks/add++s>, <s++Создать задачу++s>, userIsLogged && CMS_RIGHTS->check(df::rights::otTask, df::rights::rtCanAdd, DF_SESSION_MEMBER_DIVISION_ID_INT, DF_SESSION_MEMBER_ID_INT) ) ++v>
        </ul>
      </li>
      <li><a href='/members/'>Пользователи</a>
        <ul>
          <v++ DF_SIMPLE_MENUITEM(<s++/members/add++s>, <s++Зарегестрировать пользователя++s>, userIsLogged && CMS_RIGHTS->checkMember(df::rights::rtCanAdd, DF_SESSION_MEMBER_ID_INT) ) ++v>
        </ul>
      </li>
      <li><a href='/divisions/'>Подразделения</a>
        <ul>
          <v++ DF_SIMPLE_MENUITEM(<s++/divisions/add++s>, <s++Добавить подразделение++s>, userIsLogged && CMS_RIGHTS->checkDivision(df::rights::rtCanAdd, DF_SESSION_MEMBER_ID_INT) ) ++v>
        </ul>
      </li>
      <li>Инструменты
        <ul>
          <v++ DF_MENUITEM_NEWPAGE(<s++http://mail.ordzhonikidze-sanatorium.ru/++s>, <s++Почта++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++http://phones.ord.local/++s>, <s++Телефонный справочник++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++http://www.youtube.com/channel/UCWY3DsgnyhpvQrClXJA5hZw/live++s>, <s++Веб-камера++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++http://www.ordzhonikidze-sanatorium.ru/player/index_full.html++s>, <s++Веб-камера в полном разрешении++s>) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++http://phones.ord.local/index.php?mode=adm++s>, <s++Телефонный справочник для IT++s>, isITDivision) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++https://mail.ord.local:7071/zimbraAdmin/++s>, <s++Админка почты++s>, isITDivision) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++https://kavkaz.b2blk.megafon.ru/sc_cp_apps/login++s>, <s++Управление корпоративной мобильной связью++s>, isITDivision) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++https://wifi-manager.ord.local:8443/manage/s/default++s>, <s++Управление WiFi++s>, isITDivision) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++http://pw.ord.local/++s>, <s++Генератор паролей++s>, isITDivision) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++http://munin.ord.local/++s>, <s++Сборщик статистики++s>, isITDivision) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++http://task-test.sergos.ru/++s>, <s++Тестовая версия документооборота++s>, isITDivision) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++http://wiki.ord.local/++s>, <s++WiKi++s>, isITDivision) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++http://glpi.ord.local/++s>, <s++[GLPI] Отслеживание оборудования++s>, isITDivision) ++v>
          <v++ DF_SIMPLE_MENUITEM_NEWPAGE(<s++http://zm.ord.local/++s>, <s++[Zoneminder] Видеонаблюдение++s>, isITDivision) ++v>
        </ul>
      </li>
      <li>Мы в интернете
        <ul>
          <v++ DF_MENUITEM_NEWPAGE(<s++http://www.ordzhonikidze-sanatorium.ru/++s>, <s++Наш сайт++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++https://www.facebook.com/ordzhonikidze.kislovodsk/++s>, <s++Facebook++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++https://www.google.ru/maps/place/Санаторий+им.+Г.+К.+Орджоникидзе/++s>, <s++Google Maps++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++https://plus.google.com/u/1/+СанаторийОрджоникидзе/posts++s>, <s++Google+++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++http://ordzhonikidze.livejournal.com/++s>, <s++LiveJournal++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++https://twitter.com/sanatorium_ordz++s>, <s++Twitter++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++http://vk.com/ordzhonikidze.kislovodsk++s>, <s++ВКонтакте++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++http://ok.ru/group/51218947244223++s>, <s++Одноклассники++s>) ++v>
          <v++ DF_MENUITEM_NEWPAGE(<s++http://www.youtube.com/channel/UCWY3DsgnyhpvQrClXJA5hZw++s>, <s++Youtube++s>) ++v>
        </ul>
      </li>
      <v++ DF_DIFFERENT_MENUITEM(<s++/auth/quit++s>, <s++Выход++s>, <s++/auth++s>, <s++Вход++s>, userIsLogged) ++v>
    </ul>
    ++!>
  );
}

}
}
