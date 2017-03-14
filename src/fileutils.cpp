#include "fileutils.h"
#include <boost/filesystem.hpp>
#include <sstream>
#include <stdio.h>

#define CMS_MAX_PATH_ENTRIES 128
#define CMS_MAX_PATH_LEVEL   3

namespace df
{

bool canUseDirectory(const std::string &dir)
{
  boost::filesystem::path p(dir);
  if(boost::filesystem::exists(p))
  {
    int countDirectoryes = std::count_if(boost::filesystem::directory_iterator(p),
                                         boost::filesystem::directory_iterator(),
                                         [](const boost::filesystem::directory_entry &d)
                                         {
                                            return boost::filesystem::is_directory(d.path());
                                         } );
    return countDirectoryes < CMS_MAX_PATH_ENTRIES;
  }
  return true;
}

std::string generateDataPath(const std::string &initial)
{
  char buffer[(3*4)+3];
  for(short a = 0; a < CMS_MAX_PATH_ENTRIES; a++)
  {
    for(short b = 0; b < CMS_MAX_PATH_ENTRIES; b++)
    {
      for(short c = 0; c < CMS_MAX_PATH_ENTRIES; c++)
      {
        for(short d = 0; d < CMS_MAX_PATH_ENTRIES; d++)
        {
          sprintf(buffer, "%03d/%03d/%03d/%03d", a, b, c, d);
          if(canUseDirectory(initial + "/" + buffer))
          {
            return buffer;
          }
        }
      }
    }
  }
  return "";
}



}
