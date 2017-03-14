#include "cconnection.h"
#include <algorithm>
#include "debug.h"

namespace df
{
namespace db
{

CConnection::CConnection() : pqxx::connection(CMS_DB_CONN_STR)
{
}

CConnection::~CConnection()
{
//  m_preparedQueryes.clear();
}

//void CConnection::prepare(const std::string &name, const std::string &definition)
//{
//  CMS_DEBUG_LOG("----------- SQL preparing check --------------");
//  CMS_DEBUG_LOG("Before request prepare query: " << m_preparedQueryes.size());
//  if(std::find(m_preparedQueryes.begin(), m_preparedQueryes.end(), name) != m_preparedQueryes.end())
//  {
//    prepare_now(name);
//    CMS_DEBUG_LOG("Query already prepared");
//  }
//  else
//  {
//    CMS_DEBUG_LOG("Query already not prepared");
//    pqxx::connection::prepare(name, definition);
//    m_preparedQueryes.push_back(name);
//  }
//  CMS_DEBUG_LOG("After request prepare query: " << m_preparedQueryes.size());
//}

}
}
