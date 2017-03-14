#include "ctranslator.h"
namespace df
{
namespace model
{

CTranslator *tr_instance = nullptr;

CTranslator::CTranslator()
{
  tr_instance = this;
}

CTranslator::~CTranslator()
{

}

CTranslator *CTranslator::instance()
{
  if(tr_instance) { return tr_instance; }
  return new CTranslator();
}

void CTranslator::destroy()
{
  delete tr_instance;
  tr_instance = nullptr;
}

std::string &CTranslator::articleClass(const std::string &untranslated)
{

}

std::string &CTranslator::articleStatus(const std::string &untranslated)
{

}



}
}
