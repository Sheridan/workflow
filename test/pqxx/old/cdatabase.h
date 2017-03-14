#ifndef CDATABASE_H
#define CDATABASE_H
//#include <cppdb/frontend.h>
//#include <cppdb/backend.h>
//#include <cppdb/pool.h>
#include <pqxx/pqxx>
#include <mutex>
#include "debug.h"
#include <queue>

// connection
#define CMS_DB_HOST     "db-production.ord.local"
#define CMS_DB_DATABASE "docflow"
#define CMS_DB_LOGIN    "docflow"
#define CMS_DB_PASSWORD "docflow"
#define CMS_DB_CONN_STR "host=" CMS_DB_HOST " user=" CMS_DB_LOGIN " password=" CMS_DB_PASSWORD " dbname=" CMS_DB_DATABASE

namespace wcms
{
namespace db
{

typedef std::queue<pqxx::connection *> TDBConnections;

class CDatabase
{
  public:
    static CDatabase * instance();
    void destroy();
//    cppdb::session session();
//    bool recordExists(const std::string &table, const std::string &field, const std::string &value);
//    cppdb::session *openSession();
//    void closeSession(cppdb::session *sess, const bool &commit = true);

  private:
    CDatabase();
    virtual ~CDatabase();
    pqxx::connection *connection();
    void releaseConnection(pqxx::connection *cnn);
    pqxx::work *transaction(const std::string &name);
    void releaseTransaction(pqxx::work *trs, bool commit = true);
    TDBConnections m_inactiveConnections;
    std::mutex m_connectionMutex;
//    cppdb::pool::pointer m_pool;
//    TSessions mSessions;
};

}
}

#define CMS_DB wcms::db::CDatabase::instance()
#define CMS_SQL(_sql_name)                                cppdb::session _sql_name    = CMS_DB->session();
#define CMS_SQL_RESULT(_sql_name, _result_name, _q)       CMS_DEBUG_SQL("Query: " << _q); cppdb::result _result_name  = _sql_name << _q;
#define CMS_SQL_EXECUTE(_sql_name, _exec_name , _q)       CMS_DEBUG_SQL("Exec: "  << _q); cppdb::statement _exec_name = _sql_name << _q; _exec_name.exec();
#define CMS_SQL_RESULT_REUSE(_sql_name, _result_name, _q) CMS_DEBUG_SQL("Query: " << _q); _result_name = _sql_name << _q;
#define CMS_SQL_EXECUTE_REUSE(_sql_name, _exec_name, _q)  CMS_DEBUG_SQL("Exec: "  << _q); _exec_name   = _sql_name << _q; _exec_name.exec();
#define CMS_SQL_LAST_INSERT_ID(_result_name, _sequence)   static_cast<int>(_result_name.sequence_last(_sequence))

#define CMS_SQL_SINGLE_ROW(_sql_name, _q) \
  CMS_SQL(_sql_name##session); \
  CMS_SQL_RESULT(_sql_name##session, _sql_name, _q); \
  _sql_name->next();

#endif // CDATABASE_H
