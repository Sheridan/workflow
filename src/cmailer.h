#ifndef CMAILER_H
#define CMAILER_H
#include <string>
#include <set>
#include <mutex>

#define DF_MAIL_SERVER  "mail.sergos.ru"
#define DF_MAIL_PORT    587
#define DF_MAIL_FROM    "Документооборот <robot.task@sergos.ru>"
#define DF_MAIL_LOGIN   "robot.task@sergos.ru"
#define DF_MAIL_PW      "heir84hryherbed7wsh7"

namespace df
{
namespace mail
{

class CMailer
{
  public:
    static CMailer * instance();
    void destroy();
    void send(const std::string &to, const std::string &subject, const std::string &content);
    void send(const std::set<std::string> &to, const std::string &subject, const std::string &content);
    void send(const std::string &typeClass, const int &classID, const std::string &what, const std::string &text);

  private:
    CMailer();
    ~CMailer();
    std::mutex m_sendMutex;
};

#define DF_MAILER df::mail::CMailer::instance()

}
}
#endif // CMAILER_H
