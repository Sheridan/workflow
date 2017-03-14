#ifndef CPAGEPART_H
#define CPAGEPART_H
#include "chtmlgenerator.h"

namespace df
{
namespace ui
{

class CPagePart : public CHTMLGenerator
{
  public:
    CPagePart(cppcms::service &srv);
    ~CPagePart();
};

}
}
#endif // CPAGEPART_H
