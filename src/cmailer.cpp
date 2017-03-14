#include "cmailer.h"
#include "clogger.h"
#include "debug.h"
#include "cquery.h"
#include "global.h"
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <Poco/Net/SecureSMTPClientSession.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <thread>
#include <boost/regex.hpp>

#define DF_MAIL_MSG_CONTENT(_content) \
   "<html><head>" \
   "<link rel='stylesheet' type='text/css' href='http://" DF_SITE_URL "/media/css/style.css' />" \
   "<title>Документооборот, автоматически отправленное сообщение</title></head><body>" \
   "<h3>Автоматическое оповещение об изменениях</h3><div class='task_description'>" \
   + _content + \
   "</div></body></html>"

namespace df
{
namespace mail
{

const boost::regex siteUrlRegexp("\\b(href|src)=(['\"])/(.+)(['\"])");
const std::string siteUrlReplacement("$1=$2http://" DF_SITE_URL "/$3$4");
std::mutex threadMutex;

void doSend(std::string to, std::string subject, std::string content)
{
  threadMutex.lock();
  Poco::Net::MailMessage message;
  message.setSender(DF_MAIL_FROM);
  message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, to));
//  message.setSubject(Poco::Net::MailMessage::encodeWord(subject, "UTF-8"));
  message.setSubject(subject);
  message.setContentType("text/html; charset=UTF-8");
  message.setContent(DF_MAIL_MSG_CONTENT(boost::regex_replace(content, siteUrlRegexp, siteUrlReplacement,
                                                              boost::match_default | boost::format_perl | boost::match_any)),
                     Poco::Net::MailMessage::ENCODING_8BIT);
  try
  {
    Poco::Net::SecureSMTPClientSession session(DF_MAIL_SERVER, DF_MAIL_PORT);
    session.open();
    Poco::Net::initializeSSL();
    Poco::SharedPtr <Poco::Net::InvalidCertificateHandler> ptrHandler = new Poco::Net::AcceptCertificateHandler(false);
    Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "",
                                                                Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
    Poco::Net::SSLManager::instance().initializeClient(0, ptrHandler, ptrContext);
    try
    {
      session.login();
      if(session.startTLS())
      {
        session.login(Poco::Net::SMTPClientSession::AUTH_LOGIN, DF_MAIL_LOGIN, DF_MAIL_PW);
        session.sendMessage(message);
      }
      else { CMS_SYSTEM_LOG_ERR("Mail sender", "TLS can not be started"); }
      session.close();
    } catch (Poco::Net::SMTPException &e) { CMS_SYSTEM_LOG_ERR("Mail sender", "Mail send error: " << e.displayText()); session.close(); }
  }   catch (Poco::Net::SMTPException &e) { CMS_SYSTEM_LOG_ERR("Mail sender", "Session open error: " << e.displayText()); }
  Poco::Net::uninitializeSSL();
  CMS_DEBUG_LOG("Mail '" << subject << "' sended to " << to);
  threadMutex.unlock();
}

CMailer *mailer_instance = nullptr;

CMailer *CMailer::instance()
{
  if(mailer_instance) { return mailer_instance; }
  return new CMailer();
}

void CMailer::destroy()
{
  delete mailer_instance;
  mailer_instance = nullptr;
}

void CMailer::send(const std::string &to, const std::string &subject, const std::string &content)
{
  #ifdef CMS_DEBUG
    if(to.compare("sheridan@sergos.ru") !=0 ) { return; }
  #endif
  std::thread t(doSend, to, subject, content);
  t.detach();
}

void CMailer::send(const std::set<std::string> &to, const std::string &subject, const std::string &content)
{
  for(std::string recepient : to)
  {
    send(recepient, subject, content);
  }
}

void CMailer::send(const std::string &typeClass, const int &classID, const std::string &what, const std::string &text)
{
  m_sendMutex.lock();
  std::set<std::string> recepients;
  std::string targetName;
  if(typeClass.compare("task") == 0)
  {
    DFDB_R dbrecepients = DFDB_Q( "mail_task_recepients",
                 "select "
              /* 0*/ "prm.email, "
              /* 1*/ "crm.email, "
              /* 2*/ "ctm.email, "
              /* 3*/ "mnm.email, "
              /* 4*/ "tasks.caption "
                 "from tasks "
                 "left join divisions on divisions.id=tasks.target_division_id "
                 "left join members prm on prm.id=tasks.performer_member_id "
                 "left join members crm on crm.id=tasks.creator_member_id "
                 "left join members ctm on ctm.id=tasks.controller_member_id "
                 "left join members mnm on mnm.id=divisions.manager_member_id "
                 "where tasks.id=$1 ",
                (classID));
    for(int i=0; i<4; i++)
    {
      if(recepients.find(dbrecepients[0][i].as<std::string>()) == recepients.end())
      {
        recepients.insert(dbrecepients[0][i].as<std::string>());
      }
    }
    targetName = dbrecepients[0][4].as<std::string>();
  }
  if(!recepients.empty())
  {
    DFDB_R names = DFDB_Q( "mail_classes_names",
                 "select "
                   "concat('[', objects_classes_to_name($1), '] ') as classname, "
                   "concat('[', changelog_type_to_name($2) , '] ') as changelogname ",
                (typeClass)(what));
    DF_MAILER->send(recepients,
                    names[0][0].as<std::string>() + " " + names[0][1].as<std::string>() + " " + targetName,
                    text);
  }
  m_sendMutex.unlock();
}

CMailer::CMailer()
{}

CMailer::~CMailer()
{}

}
}
