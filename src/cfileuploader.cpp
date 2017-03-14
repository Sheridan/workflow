#include "cfileuploader.h"
#include "debug.h"
#include "clogger.h"
#include "cquery.h"
namespace df
{
namespace file
{

CFileUploader *upload_instance = nullptr;

CFileUploader::CFileUploader()
{
   upload_instance = this;
}

CFileUploader::~CFileUploader()
{

}

CFileUploader *CFileUploader::instance()
{
  if(upload_instance) { return upload_instance; }
  return new CFileUploader();
}

void CFileUploader::destroy()
{
  delete upload_instance;
  upload_instance = nullptr;
}

SUploadedFile CFileUploader::saveFile(const std::string &category, const int &categoryID, const int &ownerID, const booster::shared_ptr<cppcms::http::file> &file)
{
  SUploadedFile result;
  m_fileOperationsMutex.lock();
//  CMS_DEBUG_LOG(file->mime() << " " << file->filename() << " " << file->size());
  std::string savePath = "/data/files/" + category + "/" + std::to_string(categoryID);
  try // Пробуем создать каталог
  {
    boost::filesystem::create_directories("./" + savePath);
    try // Пробуем сохранить файл
    {
      std::string fileExtention = file->filename().substr(file->filename().find_last_of('.')+1);
      std::hash<std::string> hash_fn;
      std::size_t str_hash = hash_fn(category +
                                     std::to_string(categoryID) +
                                     std::to_string(ownerID) +
                                     file->mime() +
                                     file->filename() +
                                     std::to_string(file->size()));
      result.fileName = "original_" + std::to_string(str_hash) + "." + fileExtention;
      result.originalFileName = file->filename();
      file->save_to("./" + savePath + "/" + result.fileName);
      DFDB_R newfile = DFDB_Q("new_file_uploaded", "insert into uploaded_files ("
                                  "mime, "
                                  "file_name, "
                                  "original_file_name, "
                                  "object_class, "
                                  "class_id, "
                                  "owner_id"
                                ") values ($1, $2, $3, $4, $5, $6) returning id",
                                 (file->mime())
                                 (result.fileName)
                                 (result.originalFileName)
                                 (category)
                                 (categoryID)
                                 (ownerID));
      int fileID = CMS_SQL_LAST_INSERT_ID(newfile);
      result.fileID = fileID;
      result.saved = result.fileID > 0;
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
  m_fileOperationsMutex.unlock();
  return result;
}

void CFileUploader::sendFile(cppcms::http::response &response, const std::string &category, const int &categoryID, const int &fileID)
{
  m_fileOperationsMutex.lock();
  DFDB_R file = DFDB_Q("file_download",
            "select "
             /* 0*/ "uploaded_files.id,"
             /* 1*/ "uploaded_files.mime,"
             /* 2*/ "uploaded_files.original_file_name,"
             /* 3*/ "uploaded_files.file_name "
            "from uploaded_files "
            "where "
              "uploaded_files.object_class=$1 and "
              "uploaded_files.class_id=$2 and "
              "uploaded_files.id=$3",
            (category)(categoryID)(fileID));
  if(file.size())
  {
    std::string fileName = "data/files/" + category + "/" + std::to_string(categoryID) + "/" + file[0][3].as<std::string>("");
    std::ifstream fileStream(("./" + fileName).c_str());
    if(!fileStream)
    {
      CMS_SYSTEM_LOG_ERR("File download", "Файл " << fileName << " отсутствует на диске");
      response.status(404);
    }
    else
    {
      response.content_type(file[0][1].as<std::string>("application/octet-stream"));
      response.set_header("Content-Disposition", "inline; filename=\"" + file[0][2].as<std::string>("") + "\"");
      response.content_encoding("binary");
      response.content_length(boost::filesystem::file_size(boost::filesystem::path(fileName)));
      response.out() << fileStream.rdbuf();
    }
  }
  else
  {
    CMS_SYSTEM_LOG_ERR("File download", "Файл " << category << ":" << categoryID << ":" << fileID << " отсутствует в базе данных");
    response.status(404);
  }
  m_fileOperationsMutex.unlock();
}

}
}
