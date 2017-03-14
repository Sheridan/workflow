#ifndef CFILEUPLOADER_H
#define CFILEUPLOADER_H
#include <string>
#include <algorithm>
#include <cppcms/http_file.h>
#include <boost/filesystem.hpp>
#include <cppcms/application.h>
#include <cppcms/http_response.h>
#include <mutex>

namespace df
{
namespace file
{

struct SUploadedFile
{
    std::string fileName;
    std::string originalFileName;
    bool saved;
    int fileID;
    SUploadedFile()
    {
      fileName = "";
      originalFileName = "";
      saved = false;
      fileID = 0;
    }
};

class CFileUploader
{
  public:
    static CFileUploader * instance();
    void destroy();
    SUploadedFile saveFile(const std::string &category, const int &categoryID, const int &ownerID, const booster::shared_ptr<cppcms::http::file> &file);
    void sendFile(cppcms::http::response &response, const std::string &category, const int &categoryID, const int &fileID);
  private:
    CFileUploader();
    virtual ~CFileUploader();
    std::mutex m_fileOperationsMutex;
};

#define DF_UPLOADER df::file::CFileUploader::instance()
#define DF_UPLOADER_RESULT df::file::SUploadedFile

}
}
#endif // CFILEUPLOADER_H
