function showTaskDialog(id)
{
    showJQUIDialog
        (
            id,
            {
              width: 640,
              height: 480
            }
        );
}

function editTaskDescription(dialog, taskID)
{
    return {
        url: "/tasks/edit/"+taskID,
        value:
        {
            what: 'description',
            description: dialog.find("#task_description_"+taskID).val()
        },
        after: '/tasks/show/'+taskID
    };
}

function editTaskController(dialog, taskID)
{
    return {
        url: "/tasks/edit/"+taskID,
        value:
        {
            what: 'controller',
            controller_id: dialog.find("#task_controller_id_"+taskID).val()
        },
        after: '/tasks/show/'+taskID
    };
}

function editTaskPerformer(dialog, taskID)
{
    return {
        url: "/tasks/edit/"+taskID,
        value:
        {
            what: 'performer',
            performer_id: dialog.find("#task_performer_id_"+taskID).val()
        },
        after: '/tasks/show/'+taskID
    };
}

function editTaskControlDate(dialog, taskID)
{
    return {
        url: "/tasks/edit/"+taskID,
        value:
        {
            what: 'control_date',
            control_date: dialog.find("#control_date_"+taskID).val()
        },
        after: '/tasks/show/'+taskID
    };
}

function editTaskPriority(dialog, taskID)
{
    return {
        url: "/tasks/edit/"+taskID,
        value:
        {
            what: 'priority',
            priority: dialog.find("#task_priority_"+taskID).val()
        },
        after: '/tasks/show/'+taskID
    };
}

function editTaskCaption(dialog, taskID)
{
    return {
        url: "/tasks/edit/"+taskID,
        value:
        {
            what: 'caption',
            caption: dialog.find("#task_caption_"+taskID).val()
        },
        after: '/tasks/show/'+taskID
    };
}

function editTaskDivision(dialog, taskID)
{
    return {
        url: "/tasks/edit/"+taskID,
        value:
        {
            what: 'division',
            division_id: dialog.find("#task_division_id_"+taskID).val()
        },
        after: '/tasks/show/'+taskID
    };
}

function editTaskCoworkerAdd(dialog, taskID)
{
    return {
        url: "/tasks/edit/"+taskID,
        value:
        {
            what: 'coworker',
            coworker_id: dialog.find("#task_coworker_id_"+taskID).val(),
            operation: 'add'
        },
        after: '/tasks/show/'+taskID
    };
}

function editTaskCoworkerRemove(dialog, taskID)
{
    return {
        url: "/tasks/edit/"+taskID,
        value:
        {
            what: 'coworker',
            coworker_id: dialog.find("#task_coworker_rm_id_"+taskID).val(),
            operation: 'rm'
        },
        after: '/tasks/show/'+taskID
    };
}

function taskListRenderer(id, container, jsonData)
{
    container.html("");
    container.append("<div class='tasklist' id='task_list_" + id + "'></div>");
    container = $("#task_list_"+id);
    $.each(jsonData, function(rowIndex, row)
    {
        container.append
        ("<div class='status_" + row[13] + "' onclick=\"window.open('/tasks/show/" + row[0] + "', '_self');\">" +
                "<span class='priority priority_" + row[14] + "'></span>" +
                "<span class='identificator'>["+ row[0] + "]</span> :: " + row[4] + " :: " + row[1] +
                "<span class='progress'>" +
                    "<span style='width: " + row[17] + "%;' class='low_profile " + row[18] + "' title='" + row[19] + "'></span>" +
                "</span>" +
         "</div>");
    });
}

function taskChangelogRenderer(id, container, jsonData)
{
    container.html("");
    container.append("<table class='task_changelog' id='task_changelog_" + id + "'></table>");
    container = $("#task_changelog_"+id);
    container.append("<tr><th>Измение</th><th>Время изменения</th><th>Кто изменил</th><th>Подробно</th></tr>");
    $.each(jsonData, function(rowIndex, row)
    {
        container.append
        ("<tr>" +
            "<td>" + row[6] + "</td>" +
            "<td>" + row[1] + "</td>" +
            "<td><a href='/members/show/" + row[4] + "'>" + row[5] + "</a></td>" +
            "<td class='description'>" + row[3] + "</td>" +
          "</tr>");
    });
}
