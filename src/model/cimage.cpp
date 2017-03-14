#include "cimage.h"
#include <Magick++.h>
#include <algorithm>
#include "debug.h"
#include "cquery.h"
#include "global.h"
#include "cdatabase.h"
#include "clogger.h"
#include <boost/filesystem.hpp>

namespace df
{
namespace ui
{

SMediaData::SMediaData(const std::string &fileID,
           const std::string &mime,
           const std::string &original)
  : originalFile(original)
{
  savePath = "data/media/img/" + fileID + "/";
  std::size_t slashPos = mime.find("/");
  mimeType = mime.substr(0, slashPos);
  mimeSubType = mime.substr(slashPos+1);
}

CImage::CImage(cppcms::service &srv)
  : cppcms::application(srv)
{
  // img/id/thumbnail - превью фотографии списка изображений
  // img/id/preview   - превью фотографии на всю страницу
  // img/id/icon      - иконка фотографии
  dispatcher().assign("(\\d+)/thumbnail.jpg", &CImage::generateImageThumbnail, this, 1);
  dispatcher().assign("(\\d+)/preview.jpg"  , &CImage::generateImagePreview  , this, 1);
  dispatcher().assign("(\\d+)/icon.jpg"     , &CImage::generateImageIcon     , this, 1);
}

CImage::~CImage()
{}

void CImage::generateImageThumbnail(std::string fileID)
{
  generateImage(fileID, "thumbnail", CMS_MEDIA_CROP_THUMBNAIL_WIDTH, CMS_MEDIA_CROP_THUMBNAIL_HEIGHT);
}

void CImage::generateImagePreview(std::string fileID)
{
  generateImage(fileID, "preview", CMS_MEDIA_CROP_PREVIEW_WIDTH, CMS_MEDIA_CROP_PREVIEW_HEIGHT);
}

void CImage::generateImageIcon(std::string fileID)
{
  generateImage(fileID, "icon", CMS_MEDIA_CROP_ICON_WIDTH, CMS_MEDIA_CROP_ICON_HEIGHT);
}

void CImage::generateImage(std::string fileID,
                           const std::string &cropType,
                           const unsigned int &newWidth,
                           const unsigned int &newHeight)
{
  SMediaData mediaData = getMediaData(fileID);
  //CMS_DEBUG_LOG(mediaData.mimeType << " : " << mediaData.mimeSubType);
  if(mediaData.mimeType.compare("image") == 0 &&
     boost::filesystem::exists(boost::filesystem::path(mediaData.originalFile)))
  {
    try
    {
      resizeCropImageCentered(mediaData, cropType, newWidth, newHeight);
      //CMS_DEBUG_LOG("First call resized image " << request().path_info());
    }
    catch(std::exception &error)
    {
      CMS_WEB_LOG_ERR("Ошибка преобразования файла " << fileID << "/" << cropType << ".jpg : " << error.what());
      mediaData.originalFile = getMimeIconFile(mediaData);
      resizeCropImageCentered(mediaData, cropType, newWidth, newHeight);
    }
  }
  else
  {
    mediaData.originalFile = getMimeIconFile(mediaData);
    resizeCropImageCentered(mediaData, cropType, newWidth, newHeight);
  }
  response().set_redirect_header( "/" + mediaData.savePath + "/" + cropType + ".jpg", cppcms::http::response::see_other);
}

std::string CImage::getMimeIconFile(const SMediaData &mediaData)
{
  std::string icon = "media/img/mimetypes/" + mediaData.mimeType + "-" + mediaData.mimeSubType + ".png";
  if(!boost::filesystem::exists(boost::filesystem::path(icon)))
  {
    return "media/img/mimetypes/unknown.png";
  }
  return icon;
}

void CImage::resizeCropImageCentered(const SMediaData &mediaData, const std::string &cropType, const unsigned int &newWidth, const unsigned int &newHeight)
{
  Magick::Image image;
  image.read(mediaData.originalFile);
  unsigned int iWidth  = image.size().width();
  unsigned int iHeight = image.size().height();
  float proportionWidth  = static_cast<float>(iWidth)  / static_cast<float>(newWidth);
  float proportionHeight = static_cast<float>(iHeight) / static_cast<float>(newHeight);
  unsigned int cropTopLeftX = 0;
  unsigned int cropTopLeftY = 0;
  unsigned int cropBottomRightX = iWidth;
  unsigned int cropBottomRightY = iHeight;
  if(proportionWidth > proportionHeight)
  {
    cropTopLeftX     = (iWidth/2) - ((newWidth*proportionHeight)/2);
    cropBottomRightX = (iWidth/2) + ((newWidth*proportionHeight)/2);
    cropTopLeftY     = 0;
    cropBottomRightY = iHeight;
  }
  if(proportionHeight > proportionWidth)
  {
    cropTopLeftY     = (iHeight/2) - ((newHeight*proportionWidth)/2);
    cropBottomRightY = (iHeight/2) + ((newHeight*proportionWidth)/2);
    cropTopLeftX     = 0;
    cropBottomRightX = iWidth;
  }
  resizeCropImage(mediaData, cropType, cropTopLeftX, cropTopLeftY, cropBottomRightX, cropBottomRightY, newWidth, newHeight);
}

void CImage::resizeCropImage(const SMediaData &mediaData,
                             const std::string &cropType,
                             const unsigned int &cropTopLeftX,
                             const unsigned int &cropTopLeftY,
                             const unsigned int &cropBottomRightX,
                             const unsigned int &cropBottomRightY,
                             const unsigned int &newWidth,
                             const unsigned int &newHeight)
{
  try
  {
    Magick::Image image;
    image.read(mediaData.originalFile);
    image.resolutionUnits(Magick::PixelsPerInchResolution);
    image.density(Magick::Geometry(120, 120));
    image.quality(95);
    if(cropTopLeftX > 0 || cropTopLeftY > 0 || cropBottomRightX < image.size().width() || cropBottomRightY < image.size().height())
    {
      image.crop(Magick::Geometry(cropBottomRightX - cropTopLeftX, cropBottomRightY - cropTopLeftY, cropTopLeftX, cropTopLeftY ));
    }
    image.resize(Magick::Geometry(newWidth, newHeight));
    boost::filesystem::create_directories("./" + mediaData.savePath);
    image.write("./" + mediaData.savePath + cropType + ".jpg");
  }
  catch(std::exception &error)
  {
    CMS_WEB_LOG_ERR("Ошибка преобразования файла " << mediaData.originalFile << " в " << cropType << error.what());
    response().out() << "Ошибка преобразования файла: " << error.what();
  }
}



SMediaData CImage::getMediaData(const std::string &fileID)
{
  DFDB_R file = DFDB_Q("uploaded_file_mediadata",
              "select "
               /* 0*/ "uploaded_files.mime,"
               /* 1*/ "uploaded_files.file_name, "
               /* 2*/ "uploaded_files.object_class,"
               /* 3*/ "uploaded_files.class_id "
              "from uploaded_files "
              "where "
                "uploaded_files.id=$1 ",
              (fileID));
  if(file.size())
  {
    return SMediaData(fileID,
                      file[0][0].as<std::string>(),
                      "./data/files/" + file[0][2].as<std::string>() + "/" + file[0][3].as<std::string>() + "/" + file[0][1].as<std::string>());
  }
  return SMediaData("0", "", "");
}


}
}
