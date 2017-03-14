#include "global.h"
#include "capplication.h"
#include <cppcms/applications_pool.h>
#include <cppcms/http_response.h>
#include <Magick++.h>
#include <iostream>

int main (int argc, char* argv[])
{
  Magick::InitializeMagick("");
  try
  {
    std::cout.imbue(std::locale("ru_RU.utf8"));
    cppcms::service srv(argc,argv);
    srv.applications_pool().mount(cppcms::applications_factory<df::model::CApplication>());
    srv.run();
  }
  catch(std::exception const &e)
  {
    std::cerr<<e.what()<<std::endl;
  }
}
