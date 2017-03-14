#ifndef CIMAGE_H
#define CIMAGE_H
#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/session_interface.h>

namespace df
{
namespace ui
{

struct SMediaData
{
    std::string mimeType;
    std::string mimeSubType;
    std::string originalFile;
    std::string savePath;
    SMediaData(const std::string &fileID,
               const std::string &mime,
               const std::string &original);
};

class CImage : public cppcms::application
{
  public:
    CImage(cppcms::service &srv);
    ~CImage();
  private:
    void generateImageThumbnail(std::string fileID);
    void generateImagePreview(std::string fileID);
    void generateImageIcon(std::string fileID);
    void generateImage(std::string fileID,
                       const std::string &cropType,
                       const unsigned int &newWidth,
                       const unsigned int &newHeight);

    void resizeCropImageCentered(const SMediaData &mediaData,
                                 const std::string &cropType,
                                 const unsigned int &newWidth,
                                 const unsigned int &newHeight);
    void resizeCropImage(const SMediaData &mediaData,
                         const std::string &cropType,
                         const unsigned int &cropTopLeftX,
                         const unsigned int &cropTopLeftY,
                         const unsigned int &cropBottomRightX,
                         const unsigned int &cropBottomRightY,
                         const unsigned int &newWidth,
                         const unsigned int &newHeight);
    std::string getMimeIconFile(const SMediaData &mediaData);

    SMediaData getMediaData(const std::string &fileID);
};

}
}
#endif // CIMAGE_H
