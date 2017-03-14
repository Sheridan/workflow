#ifndef CDATALOADER_H
#define CDATALOADER_H
#include "cpagepart.h"
#include <cppcms/json.h>

namespace df
{
namespace model
{

class CDataLoader : public df::ui::CPagePart
{
    typedef std::vector<std::string> TFieldsList;
  public:
    CDataLoader(cppcms::service &srv);
    ~CDataLoader();
  protected:
    void mediaData(std::string mediaType);
    void logData();
  private:
};

}
}
#endif // CDATALOADER_H
