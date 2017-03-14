function initVisibleStatus(captionID, panelID)
{
    var caption = $("#" + captionID);
    caption.bind('click', function(e)
    {
        var e = e || window.event;
        var target = e.target || e.srcElement;
        if (this == target)
        {
            changeVisibleState(panelID);
        }

    });
}

function changeVisibleState(panelID)
{
    var element = $("#" + panelID);
    if(element.hasClass("visible"))
    {
        element.removeClass("visible");
        element.addClass("unvisible");
    } else if(element.hasClass("unvisible"))
    {
        element.removeClass("unvisible");
        element.addClass("visible");
    }
}
