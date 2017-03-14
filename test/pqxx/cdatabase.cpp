#include "cdatabase.h"
#include <stdarg.h>
namespace wcms
{
namespace db
{

CDatabase *db_instance = nullptr;

CDatabase::CDatabase()
{
  db_instance = this;
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

pqxx::connection *CDatabase::connection()
{
  pqxx::connection *cnn;
  m_connectionMutex.lock();
  if(m_inactiveConnections.empty())
  {
    cnn = new pqxx::connection(CMS_DB_CONN_STR);
  }
  else
  {
    cnn = m_inactiveConnections.front();
    m_inactiveConnections.pop();
  }
  m_connectionMutex.unlock();
  return cnn;
}

void CDatabase::releaseConnection(pqxx::connection *cnn)
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
  }
  else
  {
    trs->abort();
  }
  releaseConnection(static_cast<pqxx::connection *>(&trs->conn()));
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
