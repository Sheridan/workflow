#ifndef CDIVISIONS_H
#define CDIVISIONS_H
#include "cuserpage.h"

namespace df
{
namespace ui
{

class CDivisions : public CUserPage
{
  public:
    CDivisions(cppcms::service &srv);
    ~CDivisions();
  private:
    void composePageHead();
    void defaultPage();
    void divisionsList();
    void divisionAdd();
    void showDivision(std::string id);
    void makeDivisionControls(std::string id);
    void divisionEditForm(const std::string &action,
                        const std::string &name = std::string(),
                        const std::string &shortName = std::string(),
                        const int &parentDivisionID = 0,
                        const int &managerID = 0);
    void updateChangelog(const int &divisionID, const std::string &what, const std::string &description);
};

}
}
#endif // CDIVISIONS_H
