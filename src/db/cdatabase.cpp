#include "cdatabase.h"
#include <stdarg.h>
#include "debug.h"

namespace df
{
namespace db
{

CDatabase *db_instance = nullptr;

CDatabase::CDatabase()
{
  db_instance = this;
  #ifdef DF_DO_SQL_DEBUG
    m_connCount = 0;
  #endif
}

CDatabase::~CDatabase()
{
  m_connectionMutex.lock();
  while(!m_inactiveConnections.empty())
  {
    delete m_inactiveConnections.front();
    m_inactiveConnections.pop();
  }
  m_connectionMutex.unlock();
}

CConnection *CDatabase::connection()
{
  CConnection *cnn;
  m_connectionMutex.lock();
  if(m_inactiveConnections.empty())
  {
    cnn = new CConnection();
    #ifdef DF_DO_SQL_DEBUG
      m_connCount++;
    #endif
  }
  else
  {
    cnn = m_inactiveConnections.front();
    m_inactiveConnections.pop();
  }
  DF_SQL_DEBUG("Inactive connections after connection request: " << m_inactiveConnections.size() << ", total connections: " << m_connCount);
  m_connectionMutex.unlock();
  return cnn;
}

void CDatabase::releaseConnection(CConnection *cnn)
{
  m_connectionMutex.lock();
  m_inactiveConnections.push(cnn);
  m_connectionMutex.unlock();
}

pqxx::work *CDatabase::transaction(const std::string &name)
{
  return new pqxx::work(*connection(), name);
}

void CDatabase::releaseTransaction(pqxx::work *trs, bool commit)
{
  if(commit)
  {
    trs->commit();
    DF_SQL_DEBUG("Transaction " << trs->name() << " committed");
  }
  else
  {
    trs->abort();
    DF_SQL_DEBUG("Transaction " << trs->name() << " rollbacked");
  }
  releaseConnection(static_cast<CConnection *>(&trs->conn()));
  delete trs;
}

CDatabase *CDatabase::instance()
{
  if(db_instance) { return db_instance; }
  return new CDatabase();
}

void CDatabase::destroy()
{
  delete db_instance;
  db_instance = nullptr;
}

}
}
