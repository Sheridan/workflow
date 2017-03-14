#ifndef CCONNECTION_H
#define CCONNECTION_H
#include <pqxx/pqxx>
#include <vector>

// connection
#define CMS_DB_HOST     "db-production.ord.local"
#define CMS_DB_DATABASE "docflow"
#define CMS_DB_LOGIN    "docflow"
#define CMS_DB_PASSWORD "docflow"
#define CMS_DB_CONN_STR "host=" CMS_DB_HOST " user=" CMS_DB_LOGIN " password=" CMS_DB_PASSWORD " dbname=" CMS_DB_DATABASE

namespace df
{
namespace db
{

//typedef std::vector<std::string> TPreparedQueryes;

class CConnection : public pqxx::connection
{
  public:
    CConnection();
    virtual ~CConnection();
//    void prepare(const PGSTD::string &name, const PGSTD::string &definition);

  private:
//    TPreparedQueryes m_preparedQueryes;
};

}
}
#endif // CCONNECTION_H
