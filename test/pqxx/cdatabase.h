#ifndef CDATABASE_H
#define CDATABASE_H
#include <pqxx/pqxx>
#include <mutex>
//#include "debug.h"
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

    pqxx::connection *connection();
    void releaseConnection(pqxx::connection *cnn);

    pqxx::work *transaction(const std::string &name);
    void releaseTransaction(pqxx::work *trs, bool commit = true);

  private:
    CDatabase();
    virtual ~CDatabase();
    TDBConnections m_inactiveConnections;
    std::mutex m_connectionMutex;
};

}
}

#define CMS_DB wcms::db::CDatabase::instance()

#endif // CDATABASE_H
