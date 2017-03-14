#ifndef CTRANSLATOR_H
#define CTRANSLATOR_H
#include <string>
#include <map>

namespace df
{
namespace model
{

class CTranslator
{
  public:
    static CTranslator * instance();
    void destroy();
    std::string &articleClass(const std::string &untranslated);
    std::string &articleStatus(const std::string &untranslated);

  private:
     CTranslator();
    ~CTranslator();

  std::map<std::string, std::string> m_articleClass;
  std::map<std::string, std::string> m_articleStatus;
};

}
}
#endif // CTRANSLATOR_H
