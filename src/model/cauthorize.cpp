#include "cauthorize.h"
#include "cquery.h"
#include "html.h"
#include "crights.h"


namespace df
{
namespace ui
{

CAuthorize::CAuthorize(cppcms::service &srv)
  : CPage(srv)
{
  dispatcher().assign(""    , &CAuthorize::authorize, this);
  dispatcher().assign("quit", &CAuthorize::quit     , this);
}

CAuthorize::~CAuthorize()
{}

void CAuthorize::authorize()
{
  if(DF_REQUEST_IS_POST)
  {
    DFDB_R member = DFDB_Q("member_id_from_mail", "select id from members where email=$1",
                     (postValue("email")));
    if(member.size())
    {
      DFDB_R login = DFDB_Q("member_auth", "select id from members where email=$1 and password=$2",
                       (postValue("email"))
                       (postValue("password")));
      if(login.size())
      {
        session().reset_session();
        session().set(DF_SESSION_MEMBER_ID_KEY, login[0][0].as<int>());
        response().set_redirect_header("/", cppcms::http::response::see_other);
      }
      else
      {
        putBaseHeader(false);
        HTML
        (
          <!++
          <div class='whole_screen'>
            <div class="caption">Авторизация</div>
              Пароль неверный. Попробуйте <a href='/auth'>еще раз</a>
            </div>
          </div>
          ++!>
        );
        putBaseFooter();
      }
    }
    else
    {
      putBaseHeader(false);
      HTML
      (
        <!++
        <div class='whole_screen'>
          <div class="caption">Авторизация</div>
            Нет такого пользователя. Попробуйте <a href='/auth'>еще раз</a>
          </div>
        </div>
        ++!>
      );
      putBaseFooter();
    }
  }
  else
  {
    putBaseHeader(false);
    HTML
    (
      <!++
      <div class='whole_screen'>
        <div class="caption">Авторизация</div>
      ++!>
    );
    if(userIsLoggedOn())
    {
      DFDB_R member = DFDB_Q("member_name", "select name from members where id=$1", (DF_SESSION_MEMBER_ID_INT));
      HTML
      (
        <!++
        Вы уже вошли на сайт под именем "<v++ member[0][0].as<std::string>() ++v>"<br />
        Если хотите войти под другим именем - сначала <a href='/auth/quit'>выйдите</a>
        ++!>
      );
    }
    else
    {
      HTML
      (
        <!++
        <a href='#modalauth'>Авторизация</a>
        <div id='modalauth' class='modalDialog'>
          <div>
            <a href='#close' title='Закрыть' class='close'>X</a>
            <h2>Авторизация</h2>
            <form method='POST' action='/auth'>
              <label for='id_email'>Ваш электронный адрес</label>
              <input type="text" id="id_email" name="email"/>
              <label for='id_password'>Ваш пароль</label>
              <input type="password" id="id_password" name="password"/>
              <input type="submit" value="Вход" />
            </form>
          </div>
        </div>
        ++!>
      );
    }
    HTML(<!++ </div></div> ++!>);
    putBaseFooter();
  }
}

void CAuthorize::quit()
{
  if(DF_REQUEST_IS_POST)
  {
    CMS_RIGHTS->clear(DF_SESSION_MEMBER_ID_INT);
    session().erase(DF_SESSION_MEMBER_ID_KEY);
    session().reset_session();
    response().set_redirect_header("/auth", cppcms::http::response::see_other);
  }
  else
  {
    putBaseHeader();
    HTML
    (
      <!++
      <div class='whole_screen'>
        <div class="caption">Выход</div>
      ++!>
    );
    if(userIsLoggedOn())
    {
      DFDB_R member = DFDB_Q("member_name", "select name from members where id=$1", (DF_SESSION_MEMBER_ID_INT));
      HTML
      (
        <!++
        Вы находитесь на сайте на сайт под именем "<v++ member[0][0].as<std::string>() ++v>" <br />
        <form method='POST' action='/auth/quit'>
          <input type="submit" value="Выход" />
        </form>
        ++!>
      );
    }
    else
    {
      HTML
      (
        <!++
        Непонятно как вы сюда попали.
        Вход <a href="/auth">там</a>
        ++!>
      );
    }
    HTML(<!++ </div></div> ++!>);
    putBaseFooter();
  }
}

bool CAuthorize::composeHTMLHeaders()
{
  return true;
}


void CAuthorize::composeHead()
{
  HTML(<!++ <title>Авторизация</title> ++!>);
}

void CAuthorize::composePageHeader()
{

}

void CAuthorize::composePageFooter()
{

}

}
}
