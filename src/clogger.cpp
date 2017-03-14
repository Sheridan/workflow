#include "clogger.h"
#include "cdatabase.h"
#include "debug.h"
#include <ctime>
#include <iomanip>
#include <iostream>
namespace df
{
namespace log
{

CLogger *log_instance = nullptr;

CLogger::CLogger()
{
  m_priorityes = { "error", "warning", "info"};
  log_instance = this;
}

CLogger::~CLogger()
{

}

CLogger *CLogger::instance()
{
  if(log_instance) { return log_instance; }
  return new CLogger();
}

void CLogger::destroy()
{
  delete log_instance;
  log_instance = nullptr;
}

void CLogger::log(const std::string &urlPath, const std::string &remoteAddr, const std::string &memberID, const int &pri, std::ostream &message)
{
  std::time_t now_time = std::time(nullptr);
  std::tm tm = *std::localtime(&now_time);
  std::string msg = static_cast<std::ostringstream&>(message).str();
  CMS_DEBUG_LOG(   "[" << static_cast<int>(tm.tm_year) + 1900 << "." << tm.tm_mon+1 << "." << tm.tm_mday
                << " " << tm.tm_hour        << ":" << tm.tm_min   << ":" << tm.tm_sec << "] ["
                << m_priorityes[pri] << "] [" << urlPath << "] [" << remoteAddr << "] " << msg);
}

void CLogger::log(const std::string &subsystem, const int &pri, std::ostream &message)
{
  std::time_t now_time = std::time(nullptr);
  std::tm tm = *std::localtime(&now_time);
  std::string msg = static_cast<std::ostringstream&>(message).str();
  CMS_DEBUG_LOG(   "[" << static_cast<int>(tm.tm_year) + 1900 << "." << tm.tm_mon+1 << "." << tm.tm_mday
                << " " << tm.tm_hour        << ":" << tm.tm_min   << ":" << tm.tm_sec << "] ["
                << m_priorityes[pri] << "] [" << subsystem << "] " << msg);
}

}
}
