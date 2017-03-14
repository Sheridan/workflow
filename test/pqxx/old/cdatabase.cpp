#include "cdatabase.h"
#include <stdarg.h>
namespace wcms
{
namespace db
{

CDatabase *db_instance = nullptr;

CDatabase::CDatabase()
{
//  m_pool = cppdb::pool::create("postgresql:"
//                               "host="      CMS_DB_HOST     ";"
//                               "dbname="    CMS_DB_DATABASE ";"
//                               "user="      CMS_DB_LOGIN    ";"
//                               "password='" CMS_DB_PASSWORD "';");

  db_instance = this;
}

CDatabase::~CDatabase()
{
  m_pool.reset();
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
  return new qxx::work(connection(), name);
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
  releaseConnection(trs->conn());
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

//cppdb::session CDatabase::session()
//{
//  cppdb::session sessionSql(m_pool->open());
//  return sessionSql;
//}

//bool CDatabase::recordExists(const std::string &table, const std::string &field, const std::string &value)
//{
//  CMS_SQL(session);
//  CMS_SQL_RESULT(session, sr, "select count(*) as cnt from " + table + " where " + field + "=?;"
//                          << value);
//  sr.next();
//  return sr.get<int>(0) > 0;
//}

//cppdb::session *CDatabase::openSession()
//{
//  cppdb::session sessionSql = new  cppdb::session(m_pool->open());
//  mSessions.push_back(sessionSql);
//  return sessionSql;
//}

//void CDatabase::closeSession(cppdb::session *sess, const bool &commit)
//{
//  mSessions.
//}

}
}
