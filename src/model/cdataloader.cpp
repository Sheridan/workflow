#include "cdataloader.h"
#include "cdatabase.h"
#include "html.h"
#include "debug.h"
#include "clogger.h"

namespace df
{
namespace model
{

CDataLoader::CDataLoader(cppcms::service &srv)
  : CPagePart(srv)
{
  dispatcher().assign("media/(\\w+)"            , &CDataLoader::mediaData         , this, 1   );
  dispatcher().assign("log"                     , &CDataLoader::logData           , this      );
}

CDataLoader::~CDataLoader()
{}


void CDataLoader::mediaData(std::string mediaType)
{
//  if(request().request_method() == "POST")
//  {
//    CMS_DEBUG_POST;
//    int firstRecord  = request().post().find("offset")       != request().post().end() ? stoi(request().post("offset"))       : 0;
//    int itemsPerPage = request().post().find("itemsPerPage") != request().post().end() ? stoi(request().post("itemsPerPage")) : 25;
//    CMS_SQL(sql);
//    CMS_SQL_RESULT(sql, sr,   "select "
//                                "id,"
//                                "path,"
//                                "caption,"
//                                "url_part "
//                               "from media where type=? order by id desc "
//                               "limit ? offset ?;"
//                            << mediaType
//                            << itemsPerPage
//                            << firstRecord);
//    cppcms::json::value data;
//    int row = 0;
//    while(sr.next())
//    {
//      data["data"][row][0] = sr.get<int>(0);
//      data["data"][row][1] = sr.get<std::string>(1);
//      data["data"][row][2] = sr.get<std::string>(2);
//      data["data"][row][3] = sr.get<std::string>(3);
//      row++;
//    }
//    data["firstRecord"] = firstRecord;
//    data["lastRecord"]  = firstRecord+row;
//    CMS_SQL_RESULT(sql, srt, "select count(id) from media where type=?;" << mediaType); srt.next();
//    data["totalRecords"] = srt.get<int>(0);
//    response().out() << data;
//  }
//  else
//  {
//    response().out() << "Вы что то делаете не так";
//    CMS_WEB_LOG_ERR("Запрос данных медиа не через POST");
//  }
}

void CDataLoader::logData()
{
//  if(request().request_method() == "POST")
//  {
//    CMS_DEBUG_POST;
//    int firstRecord  = request().post().find("offset")       != request().post().end() ? stoi(request().post("offset"))       : 0;
//    int itemsPerPage = request().post().find("itemsPerPage") != request().post().end() ? stoi(request().post("itemsPerPage")) : 50;
//    std::string orderBy = request().post("orderBy").compare("member_id") == 0 ? "members.name" :  "log." + request().post("orderBy");
//    std::string order = request().post("order");
//    CMS_SQL(sql);
//    CMS_SQL_RESULT(sql, sr,    "select "
//                                 "to_char(log.action_timestamp, 'YYYY.MM.DD HH24:MI:SS') as tst,"
//                                 "log.member_id,"
//                                 "log.priority,"
//                                 "log.addr,"
//                                 "log.message,"
//                                 "members.name "
//                               "from log "
//                               "left join members on members.id=log.member_id order by "
//                               + orderBy + " " + order +
//                               " limit ? offset ?;"
//                            << itemsPerPage
//                            << firstRecord);
//    cppcms::json::value data;
//    int row = 0;
//    while(sr.next())
//    {
//      data["data"][row][0] = sr.get<std::string>(0);
//      data["data"][row][1] = sr.get<int>(1);
//      data["data"][row][2] = sr.get<std::string>(2);
//      data["data"][row][3] = sr.get<std::string>(3);
//      data["data"][row][4] = sr.get<std::string>(4);
//      data["data"][row][5] = sr.get<std::string>(5);
//      row++;
//    }
//    data["firstRecord"] = firstRecord;
//    data["lastRecord"]  = firstRecord+row;
//    CMS_SQL_RESULT(sql, srt, "select count(*) from log;"); srt.next();
//    data["totalRecords"] = srt.get<int>(0);
//    response().out() << data;
//  }
//  else
//  {
//    response().out() << "Вы что то делаете не так";
//    CMS_WEB_LOG_ERR("Запрос данных лога не через POST");
//  }
}




}
}
