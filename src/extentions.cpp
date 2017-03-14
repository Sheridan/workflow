#include <stdlib.h>
#include "extentions.h"
#include <boost/algorithm/string.hpp>

std::ostream& operator<<(std::ostream& os, const std::tm& tm)
{
  os << "[" << static_cast<int>(tm.tm_year) + 1900 << "." << tm.tm_mon+1 << "." << tm.tm_mday
     << " " << tm.tm_hour        << ":" << tm.tm_min   << ":" << tm.tm_sec << "]";
  return os;
}

namespace df
{


std::string formatPgTimeInterval(const std::string &interval)
{
  std::vector<std::string> splitted;
  boost::split(splitted, interval, boost::is_any_of("|"));
  std::string result = " ";
  if(atoi(splitted[0].c_str())) { result += splitted[0] + " г. "; }
  if(atoi(splitted[1].c_str())) { result += splitted[1] + " м. "; }
  if(atoi(splitted[2].c_str())) { result += splitted[2] + " д. "; }
  result += splitted[3] + ":" + splitted[4] + ":" + splitted[5];
  return result;
}

}
