#include "cquery.h"
#include "debug.h"
#include "cconnection.h"
#include <map>

namespace df
{
namespace db
{

#ifdef DF_DO_SQL_DEBUG
  typedef std::map<std::string, std::string> TQCache;
  TQCache __dfdbqcache;
#endif

CQuery::CQuery()
{
  m_commit = true;
  DF_SQL_DEBUG("++++++++++++++++++++++++++++++++++++++++++++++++++++ SQL ++++++++++++++++++++++++++++++++++++++++++++++++++++");
}

CQuery::~CQuery()
{
  CMS_DB->releaseTransaction(m_transaction, m_commit);
  DF_SQL_DEBUG("---------------------------------------------------- SQL ----------------------------------------------------");
}

void CQuery::prepareQuery(const std::string &name, const std::string &sql)
{
  m_name = name;
  #ifdef DF_DO_SQL_DEBUG
    //CMS_DEBUG_LOG("----------- SQL names check --------------");
    if(__dfdbqcache.count(name) > 0)
    {
      //CMS_DEBUG_LOG("Query " << name << " already in cache");
      for (auto& val : __dfdbqcache)
      {
        if(val.second.compare(sql) == 0 && val.first.compare(name) != 0)
        {
          DF_SQL_DEBUG(" -----------============------------- Equal queryes with different names ("<< val.first << ", " << name << ")");
        }
      }
    }
    else
    {
      //CMS_DEBUG_LOG("Adding query " << name << " to cache");
      __dfdbqcache[name] = sql;
    }
    DF_SQL_DEBUG("Queryes in cache: " << __dfdbqcache.size());
  #endif
  m_transaction = CMS_DB->transaction(m_name);
  if(!m_transaction->prepared(m_name).exists())
  {
    static_cast<CConnection *>(&m_transaction->conn())->prepare(m_name, sql);
  }
}

pqxx::prepare::invocation CQuery::go()
{
  return m_transaction->prepared(m_name);
}

void CQuery::setCommit(const bool &commit)
{
  m_commit = commit;
}

}
}
