#ifndef CQUERY_H
#define CQUERY_H
#include "cdatabase.h"
#include "debug.h"
#include <string>

namespace df
{
namespace db
{

class CQuery
{
  public:
    CQuery();
    virtual ~CQuery();
    void prepareQuery(const std::string &name, const std::string &sql);
    pqxx::prepare::invocation go();
    void setCommit(const bool &commit = true);
  private:
    pqxx::work *m_transaction;
    std::string m_name;
    bool m_commit;
};

class CResult
{
  public:
    CResult() {}
    virtual ~CResult(){}

};

}
}



#define DFDB_R pqxx::result

#define DFDB_Q(_sql_name, _query, _values) \
  [&]() -> DFDB_R\
  { \
    df::db::CQuery *q = new df::db::CQuery(); \
    try \
    { \
      DF_SQL_DEBUG(_sql_name << std::endl << _query << std::endl << "Variables: " << #_values); \
      CMS_DEBUG_LOG_STREAM(_values); \
      DF_SQL_DEBUG(""); \
      q->prepareQuery(_sql_name, _query); \
      DFDB_R r = q->go()_values.exec(); \
      delete q; \
      return r; \
    } \
    catch (const std::exception &e) \
    { \
      q->setCommit(false); \
      delete q; \
      std::cerr << "[!!!!!!!!!!!!!!!!!!!!]" << e.what() << std::endl; \
      DF_SQL_DEBUG(_sql_name << std::endl << _query << std::endl << "Variables: " << #_values); \
      abort(); \
    } \
  }()

#define DFDB_QLOOP(_q_result,_row_name) for (DFDB_R::tuple _row_name : _q_result)
#define CMS_SQL_LAST_INSERT_ID(_q_result) _q_result[0][0].as<int>()
#define DF_SQL_TEXT(...) #__VA_ARGS__

#endif // CQUERY_H
