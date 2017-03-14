
function setMemberRight(mid, obj, right)
{
    var allow = $("#or_"+obj+"_"+right).prop("checked");
    $.post("/members/setrights/"+mid,
    {
      object_class: obj,
      right_type  : right,
      allow       : allow
    });
}

function memberChangelogRenderer(id, container, jsonData)
{
    container.html("");
    container.append("<table class='task_changelog' id='member_changelog_" + id + "'></table>");
    container = $("#member_changelog_"+id);
    container.append("<tr><th>Измение</th><th>Время изменения</th><th>Ссылка на объект</th><th>Подробно</th></tr>");
    $.each(jsonData, function(rowIndex, row)
    {
        container.append
        ("<tr>" +
            "<td>" + row[7] + "</td>" +
            "<td>" + row[1] + "</td>" +
            "<td><a href='/" + row[4] + "s/show/" + row[5] + "'>" + row[6] + "</a></td>" +
            "<td class='description'>" + row[3] + "</td>" +
        "</tr>");
    });
}
