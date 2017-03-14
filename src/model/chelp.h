#ifndef CHELP_H
#define CHELP_H
#include "cuserpage.h"

#define DF_HELP_FUNCTIONNAME(_keyword) _keyword##Help
#define DF_HELP_DECLARE(_keyword) void DF_HELP_FUNCTIONNAME(_keyword)()

namespace df
{
namespace ui
{

class CHelp : public CUserPage
{
  public:
    CHelp(cppcms::service &srv);
    ~CHelp();
  private:
    void composePageHead();
    void defaultPage();
    DF_HELP_DECLARE(taskID);
    DF_HELP_DECLARE(taskCaption);
    DF_HELP_DECLARE(taskPriority);
    DF_HELP_DECLARE(taskCreationDate);
    DF_HELP_DECLARE(taskControlDate);
    DF_HELP_DECLARE(taskCloseDate);
    DF_HELP_DECLARE(taskTotalTime);
    DF_HELP_DECLARE(taskTimeFromStart);
    DF_HELP_DECLARE(taskTimeToEnd);
    DF_HELP_DECLARE(taskTimeFromEnd);
    DF_HELP_DECLARE(taskDivision);
    DF_HELP_DECLARE(taskCreator);
    DF_HELP_DECLARE(taskController);
    DF_HELP_DECLARE(taskPerformer);
    DF_HELP_DECLARE(taskDescription);
    DF_HELP_DECLARE(taskProgress);
    DF_HELP_DECLARE(taskFiles);
    DF_HELP_DECLARE(taskDone);
    DF_HELP_DECLARE(taskDoneNot);
    DF_HELP_DECLARE(taskDayInWork);
    DF_HELP_DECLARE(taskDayControl);
    DF_HELP_DECLARE(taskDayStarted);
    DF_HELP_DECLARE(taskDayFinished);
    DF_HELP_DECLARE(taskForMe);
    DF_HELP_DECLARE(taskMyControl);
    DF_HELP_DECLARE(taskInWork);
    DF_HELP_DECLARE(taskFinished);

    DF_HELP_DECLARE(commentText);

    DF_HELP_DECLARE(tabCommentView);
    DF_HELP_DECLARE(tabCommentAdd);
    DF_HELP_DECLARE(tabHistory);
    DF_HELP_DECLARE(tabControl);
    DF_HELP_DECLARE(tabAddFiles);

    DF_HELP_DECLARE(memberFio);
    DF_HELP_DECLARE(memberPassword);
    DF_HELP_DECLARE(memberEMail);
    DF_HELP_DECLARE(memberDivision);

    DF_HELP_DECLARE(divisionLongName);
    DF_HELP_DECLARE(divisionShortName);
    DF_HELP_DECLARE(divisionParentDivision);
    DF_HELP_DECLARE(divisionManager);

    DF_HELP_DECLARE(maintenanceWorkClassCaption);
    DF_HELP_DECLARE(maintenanceWorkClassDescrition);
    DF_HELP_DECLARE(maintenanceWorkClassController);
    DF_HELP_DECLARE(maintenanceWorkClassTargetDivision);
};

}
}
#endif // CHELP_H
