#ifndef CQUERY_H
#define CQUERY_H
#include "cdatabase.h"
#include <string>
namespace wcms
{
namespace db
{

class CQuery
{
  public:
    CQuery(const std::string &name, const std::string &sql);
    virtual ~CQuery();
    pqxx::prepare::invocation go();
  private:
    pqxx::work *m_transaction;
    std::string m_name;
};

}
}

#define CMS_DB_Q(_sql_name, _query, _values) \
  pqxx::result _sql_name = [&]() -> pqxx::result\
  { \
    std::cout << #_sql_name << std::endl << _query << std::endl << #_values <<  std::endl; \
    try \
    { \
      wcms::db::CQuery q(#_sql_name, _query); \
      return q.go()_values.exec(); \
    } \
    catch (const std::exception &e) { std::cerr << e.what() << std::endl; abort(); }\
  }()

#define CMS_DB_ROW(_sql_name,_row_name) pqxx::result::const_iterator _row_name = _sql_name.begin()
#define CMS_DB_QLOOP(_sql_name,_row_name) for (CMS_DB_ROW(_sql_name,_row_name); _row_name != _sql_name.end(); ++_row_name)


#endif // CQUERY_H
