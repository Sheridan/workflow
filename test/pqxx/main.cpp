#include <iostream>
#include "cdatabase.h"
#include "cquery.h"

int main()
{


  CMS_DB_Q(divisions, "SELECT id, short_name from divisions", );

  CMS_DB_QLOOP(divisions, drow)
  {
    std::cout << drow[0].as<int>() << " " << drow[1].as<std::string>() << std::endl;
    CMS_DB_Q(members, "select id, name from members where division_id=$1", (drow[0].as<int>()));
    CMS_DB_QLOOP(members, mrow)
    {
      std::cout << "\t" << mrow[0].as<int>() << " " << mrow[1].as<std::string>() << std::endl;
    }
  }
  
  CMS_DB_Q(division, "SELECT id, short_name from divisions where id=$1", (1));
  CMS_DB_ROW(division, odrow);
  std::cout << odrow[0].as<int>() << " " << odrow[1].as<std::string>() << std::endl;
  
  CMS_DB->destroy();
  return 0;
}
