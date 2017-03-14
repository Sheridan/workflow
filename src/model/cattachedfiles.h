#ifndef CATTACHEDFILES_H
#define CATTACHEDFILES_H
#include "cuserpage.h"
namespace df
{
namespace ui
{

enum EFileListType
{
  fltTable,
  fltThumbnails
};

class CAttachedFiles : public CUserPage
{
  public:
    CAttachedFiles(cppcms::service &srv);
    ~CAttachedFiles();

  private:
    void composePageHead();
    void upload(std::string category, std::string categoryID);
    void download(std::string fileID);
    void list(std::string category, std::string categoryID);
    void changeListType(std::string type);
    EFileListType fileListType();
};

}
}
#endif // CATTACHEDFILES_H
