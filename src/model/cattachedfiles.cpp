#include "cattachedfiles.h"
#include "cquery.h"
#include "html.h"
#include "debug.h"
#include "clogger.h"
#include <boost/filesystem.hpp>
#include <cppcms/http_file.h>

namespace df
{
namespace ui
{

CAttachedFiles::CAttachedFiles(cppcms::service &srv)
  : CUserPage(srv)
{
  dispatcher().assign("upload/(\\w+)/(\\d+)", &CAttachedFiles::upload        , this, 1, 2   ); // class (task, etc), class_id
  dispatcher().assign("download/(\\d+)"     , &CAttachedFiles::download      , this, 1      ); // file id
  dispatcher().assign("list/(\\w+)/(\\d+)"  , &CAttachedFiles::list          , this, 1, 2   ); // class (task, etc), class_id
  dispatcher().assign("chtype/(\\w+)"       , &CAttachedFiles::changeListType, this, 1      ); // list type (table, thumbs)
}

CAttachedFiles::~CAttachedFiles()
{}

void CAttachedFiles::composePageHead()
{
  HTML
  (
    <!++
    <title>Файлы</title>
    ++!>
  );
}

EFileListType CAttachedFiles::fileListType()
{
  if(session().is_set("upload_file_list_type"))
  {
    if(session()["upload_file_list_type"].compare("table") == 0)
    {
      return fltTable;
    }
    return fltThumbnails;
  }
  return fltTable;
}

void CAttachedFiles::upload(std::string category, std::string categoryID)
{
  DF_STOP_IF_NO_AUTH;
  if(request().request_method() == "POST")
  {
    booster::shared_ptr<cppcms::http::file> file = request().files()[0];
    std::string savePath = "/data/files/" + category + "/" + categoryID;
    try // Пробуем создать каталог
    {
      boost::filesystem::create_directories("./" + savePath);
      try // Пробуем сохранить файл
      {
        std::string originalFileName = file->filename();
        std::string fileExtention = originalFileName.substr(file->filename().find_last_of('.')+1);
        std::hash<std::string> hash_fn;
        std::size_t str_hash = hash_fn(category +
                                       categoryID +
                                       DF_SESSION_MEMBER_ID_STR +
                                       file->mime() +
                                       originalFileName +
                                       std::to_string(file->size()));
        std::string fileName = "original_" + std::to_string(str_hash) + "." + fileExtention;
        file->save_to("./" + savePath + "/" + fileName);
        DFDB_R newfile = DFDB_Q("new_file_uploaded", "insert into uploaded_files ("
                                    "mime, "
                                    "file_name, "
                                    "original_file_name, "
                                    "object_class, "
                                    "class_id, "
                                    "owner_id"
                                  ") values ($1, $2, $3, $4, $5, $6) returning id",
                                   (file->mime())
                                   (fileName)
                                   (originalFileName)
                                   (category)
                                   (categoryID)
                                   (DF_SESSION_MEMBER_ID_INT));
        int fileID = CMS_SQL_LAST_INSERT_ID(newfile);
        updateChangelog(category, atoi(categoryID.c_str()), "file_uploaded", "Загружен файл "
                                        "<a href='/file/download/" + std::to_string(fileID) + "'>" + originalFileName + "</a>" );
      }
      catch(std::exception &error)
      {
        CMS_SYSTEM_LOG_ERR("File upload", std::string("Ошибка сохранения файла на диск: ") + error.what());
      }
    }
    catch(std::exception &error)
    {
      CMS_SYSTEM_LOG_ERR("File upload", std::string("Ошибка создания вложенных директорий: ") + error.what());
    }
  }
}

void CAttachedFiles::download(std::string fileID)
{
  DF_STOP_IF_NO_AUTH;
  DFDB_R file = DFDB_Q("file_download",
            "select "
             /* 0*/ "uploaded_files.id,"
             /* 1*/ "uploaded_files.mime,"
             /* 2*/ "uploaded_files.original_file_name,"
             /* 3*/ "uploaded_files.file_name,"
             /* 4*/ "uploaded_files.object_class,"
             /* 5*/ "uploaded_files.class_id "
            "from uploaded_files "
            "where "
              "uploaded_files.id=$1",
            (fileID));
  if(file.size())
  {
    std::string fileName = "data/files/" + file[0][4].as<std::string>() + "/" + file[0][5].as<std::string>() + "/" + file[0][3].as<std::string>();
    std::ifstream fileStream(("./" + fileName).c_str());
    if(!fileStream)
    {
      CMS_SYSTEM_LOG_ERR("File download", "Файл " << fileName << " отсутствует на диске");
      response().status(404);
    }
    else
    {
      response().content_type(file[0][1].as<std::string>("application/octet-stream"));
      response().set_header("Content-Disposition", "inline; filename=\"" + file[0][2].as<std::string>("") + "\"");
      response().content_encoding("binary");
      response().content_length(boost::filesystem::file_size(boost::filesystem::path(fileName)));
      response().out() << fileStream.rdbuf();
    }
  }
  else
  {
    CMS_SYSTEM_LOG_ERR("File download", "Файл "  << fileID << " отсутствует в базе данных");
    response().status(404);
  }
}

void CAttachedFiles::list(std::string category, std::string categoryID)
{
  DF_STOP_IF_NO_AUTH;
  HTML
  (
     <!++
        <div class='filelist_controls'>
        <img class='icon_button' src='/media/img/icons/drag.svg' onclick='changeListType(<dq++/file/list/<v++ category ++v>/<v++ categoryID ++v> ++dq>, <dq++table++dq>, this)' />
        <img class='icon_button' src='/media/img/icons/grid.svg' onclick='changeListType(<dq++/file/list/<v++ category ++v>/<v++ categoryID ++v> ++dq>, <dq++thumbnails++dq>, this)'/>
        </div>
        <div class='filelist' id='filelist_<v++ category ++v>_<v++ categoryID ++v>'>
     ++!>
  );
  DFDB_R uploadedFiles = DFDB_Q("uploaded_files",
              "select "
               /* 0*/ "uploaded_files.id,"
               /* 1*/ "to_char(uploaded_files.uploaded_datetime, 'DD.MM.YYYY HH24:MI'),"
               /* 2*/ "uploaded_files.mime,"
               /* 3*/ "uploaded_files.original_file_name,"
               /* 4*/ "members.id,"
               /* 5*/ "members.name, "
               /* 6*/ "uploaded_files.file_name, "
               /* 7*/ "to_char(uploaded_files.uploaded_datetime, 'DD.MM.YYYY HH24') "
              "from uploaded_files "
              "left join members on uploaded_files.owner_id = members.id "
              "where "
                "uploaded_files.object_class=$1 and "
                "uploaded_files.class_id=$2 "
                "order by uploaded_files.uploaded_datetime desc ",
              (category)(categoryID));
  if(uploadedFiles.size())
  {
    boost::filesystem::path fileName;
    std::string fileSize, block;
    std::string blockVisibility = "visible";
    EFileListType lType = fileListType();
    DFDB_QLOOP(uploadedFiles, uFile)
    {
      fileName = boost::filesystem::path("./data/files/" + category + "/" + categoryID + "/" + uFile[6].as<std::string>(""));
      fileSize = boost::filesystem::exists(fileName) ? std::to_string(boost::filesystem::file_size(fileName)) : "Отсутствует";
      if(block.compare(uFile[7].as<std::string>()) != 0)
      {
        if(!block.empty())
        {
          switch (lType)
          {
            case fltTable:
            {
              HTML
              (
                 <!++
                    </table>
                 ++!>
              );
            } break;
            case fltThumbnails:
            {
              HTML
              (
                 <!++
                    </div>
                 ++!>
              );
            } break;
          }
          HTML
          (
             <!++
                <++panel>
             ++!>
          );
          blockVisibility = "unvisible";
        }
        block = uFile[7].as<std::string>();
        HTML
        (
           <!++
              <panel++ [<v++ block ++v>:00] [ ] [<v++ blockVisibility ++v>] [task_uploaded_files_<v++ uFile[0].as<std::string>() ++v>]>
           ++!>
        );
        switch (lType)
        {
          case fltTable:
          {
            HTML
            (
               <!++
                <table class='filelist'>
                  <tr>
                    <th>Имя файла</th>
                    <th>Кем загружен</th>
                    <th>Когда загружен</th>
                    <th>Размер</th>
                  </tr>
               ++!>
            );
          } break;
          case fltThumbnails:
          {
            HTML
            (
               <!++
                  <div class='thumbnail_container'>
               ++!>
            );
          } break;
        }
      }
      switch (lType)
      {
        case fltTable:
        {
          HTML
          (
             <!++
                <tr>
                  <td class='name'><a href='/file/download/<v++ uFile[0].as<std::string>() ++v>'><v++ uFile[3].as<std::string>() ++v></a></td>
                  <td><a href='/members/show/<v++ uFile[4].as<std::string>() ++v>'><v++ uFile[5].as<std::string>() ++v></a></td>
                  <td><v++ uFile[1].as<std::string>() ++v></td>
                  <td class='size'><v++ fileSize ++v></td>
                </tr>
             ++!>
          );
        } break;
        case fltThumbnails:
        {
          HTML
          (
             <!++
                <div class='thumbnail'>
                  <div class='image_container_thumbnail'>
                    <a href='/file/download/<v++ uFile[0].as<std::string>() ++v>'>
                      <img class='thumbnail' src='/data/media/img/<v++ uFile[0].as<std::string>() ++v>/thumbnail.jpg' />
                    </a>
                  </div>
                  <div class='description'>
                    <a href='/file/download/<v++ uFile[0].as<std::string>() ++v>'><v++ uFile[3].as<std::string>() ++v></a><br />
                    <a href='/members/show/<v++ uFile[4].as<std::string>() ++v>'><v++ uFile[5].as<std::string>() ++v></a>
                  </div>
                </div>
             ++!>
          );
        } break;
      }
    }
  }
  HTML
  (
     <!++
        </div>
     ++!>
  );
}

void CAttachedFiles::changeListType(std::string type)
{
  DF_STOP_IF_NO_AUTH;
  if(request().request_method() == "POST")
  {
    if(type.compare("table") == 0 ||
       type.compare("thumbnails") == 0)
    {
      session().set("upload_file_list_type", type);
    }
    else
    {
      session().set("upload_file_list_type", std::string("table"));
    }
  }
  HTML
  (
     <!++
        .
     ++!>
  );
}



}
}
