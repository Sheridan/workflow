#include "cquery.h"
namespace wcms
{
namespace db
{

CQuery::CQuery(const std::string &name, const std::string &sql)
  : m_name(name)
{
  m_transaction = CMS_DB->transaction(m_name);
  m_transaction->conn().prepare(m_name, sql);
}

CQuery::~CQuery()
{
  CMS_DB->releaseTransaction(m_transaction);
}

pqxx::prepare::invocation CQuery::go()
{
  return m_transaction->prepared(m_name);
}

}
}
