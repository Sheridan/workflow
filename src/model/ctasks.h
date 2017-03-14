#ifndef CTASKS_H
#define CTASKS_H
#include "cuserpage.h"

namespace df
{
namespace ui
{

class CTasks  : public CUserPage
{
  public:
    CTasks(cppcms::service &srv);
    ~CTasks();

  private:
    void composePageHead();
    void defaultPage();
    void taskList();
    std::string taskListFilterTaskStatus();
    std::string taskListOrder();


    void addTask();
    void editTask(std::string id);
    void showTask(std::string id);
    void dayTotal(std::string year, std::string month, std::string day);
    void showChangelog(std::string id);
    void addComment(std::string id);
    void addComment(const std::string &id, const std::string &comment, const bool &redirect, const bool &addChangelogRecord);
    void listComments(std::string id);
    void listTaskCoworkers(const int &taskID, const int &taskRecordDivisionID);
    void makeCommentForm(const int &taskID,
                         const int &taskRecordDivisionID,
                         const std::string &taskStatus);
    std::string  updateChangelog(const int &taskID, const std::string &what, const std::string &description);

    void makeEditDescription(const int &taskID,
                             const int &taskRecordDivisionID,
                             const std::string &text);
    void makeEditController(const int &taskID,
                            const int &taskRecordDivisionID,
                            const int &controllerMemberID);
    void makeEditPerformer(const int &taskID,
                           const int &taskRecordDivisionID,
                           const int &performerMemberID);
    void makeEditControlDate(const int &taskID,
                             const int &taskRecordDivisionID,
                             const std::string &dateTime);
    void makeEditPriority(const int &taskID,
                          const int &taskRecordDivisionID,
                          const std::string &priority);
    void makeEditCaption(const int &taskID,
                          const int &taskRecordDivisionID,
                          const std::string &caption);
    void makeEditDivision(const int &taskID,
                          const int &taskRecordDivisionID,
                          const int &divisionID);

    void controlledMembersSelectBox(const int &taskID = 0, const int &currentControllerMemberID = 0);
    void performerMembersSelectBox(const int &taskID = 0, const int &currentPerformerMemberID = 0);
    void controlDateEdit(const int &taskID = 0, const std::string &currentDateTime = std::string(""));
    void prioritySelectBox(const int &taskID = 0, std::string currentPriority = "default");
    void makeTaskControls(const int &taskID,
                          const int &taskRecordDivisionID,
                          const std::string &taskStatus);
    void makeTaskUploadDialog(const int &taskID,
                          const int &taskRecordDivisionID,
                          const std::string &taskStatus);
};

}
}
#endif // CTASKS_H
