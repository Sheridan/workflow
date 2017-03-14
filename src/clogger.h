#ifndef CLOGGER_H
#define CLOGGER_H
#include <cppcms/http_context.h>
#include <cppcms/http_request.h>

#include <string>
#include <sstream>
#include <vector>

namespace df
{
namespace log
{

class CLogger
{
  public:
    static CLogger * instance();
    void destroy();
    void log(const std::string &urlPath, const std::string &remoteAddr, const std::string &memberID, const int &pri, std::ostream &message);
    void log(const std::string &subsystem, const int &pri, std::ostream &message);
  private:
    CLogger();
    virtual ~CLogger();
    std::vector<std::string> m_priorityes;
};

#define CMS_WEB_LOG(_pri, _msg) \
  df::log::CLogger::instance()->log(request().path_info(), request().remote_addr(), \
                                    (session().is_set("member") && !session()["member"].empty()) ? session()["member"] : "0", \
                                    _pri, \
                                    std::stringstream() << _msg);

#define CMS_WEB_LOG_ERR(_msg) CMS_WEB_LOG(0, _msg)
#define CMS_WEB_LOG_WRN(_msg) CMS_WEB_LOG(1, _msg)
#define CMS_WEB_LOG_NFO(_msg) CMS_WEB_LOG(2, _msg)


#define CMS_SYSTEM_LOG(_subsystem, _pri, _msg) \
  df::log::CLogger::instance()->log(_subsystem, \
                                    _pri, \
                                    std::stringstream() << _msg);

#define CMS_SYSTEM_LOG_ERR(_subsystem, _msg) CMS_SYSTEM_LOG(_subsystem, 0, _msg)
#define CMS_SYSTEM_LOG_WRN(_subsystem, _msg) CMS_SYSTEM_LOG(_subsystem, 1, _msg)
#define CMS_SYSTEM_LOG_NFO(_subsystem, _msg) CMS_SYSTEM_LOG(_subsystem, 2, _msg)


}
}
#endif // CLOGGER_H
