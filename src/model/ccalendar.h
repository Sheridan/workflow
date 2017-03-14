#ifndef CCALENDAR_H
#define CCALENDAR_H
#include "cuserpage.h"
#include <ctime>

namespace df
{
namespace ui
{

class CCalendar : public CUserPage
{
  public:
    CCalendar(cppcms::service &srv);
    ~CCalendar();
  private:
    void composePageHead();
    void defaultPage();
    void showMonth(const int &year, const int &month);
    void showMonth(std::string year, std::string month);
    void showDay(std::string year, std::string month, std::string day);
    void showDay(const int &year, const int &month, const int &day);

    const char * m_monthNames[12] =
    {
      "Январь"  , "Февраль", "Март"  , "Апрель" ,
      "Май"     , "Июнь"   , "Июль"  , "Август" ,
      "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"
    };
    const char * m_weekNames[7] =
    {
      "Понедельник", "Вторник", "Среда"      , "Четверг",
      "Пятница"    , "Суббота", "Воскресение"
    };
};

}
}
#endif // CCALENDAR_H

