#ifndef CDATABASE_H
#define CDATABASE_H
#include <pqxx/pqxx>
#include <mutex>
#include <queue>
#include "cconnection.h"
#include "debug.h"


namespace df
{
namespace db
{

typedef std::queue<CConnection *> TDBConnections;

class CDatabase
{
  public:
    static CDatabase * instance();
    void destroy();

    CConnection *connection();
    void releaseConnection(CConnection *cnn);

    pqxx::work *transaction(const std::string &name);
    void releaseTransaction(pqxx::work *trs, bool commit = true);

  private:
    CDatabase();
    virtual ~CDatabase();
    TDBConnections m_inactiveConnections;
    std::mutex m_connectionMutex;
  #ifdef DF_DO_SQL_DEBUG
    int m_connCount;
  #endif
};

}
}

#define CMS_DB df::db::CDatabase::instance()

#endif // CDATABASE_H
