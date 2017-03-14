var pageControls = {};

function initPageControl(pageControlID)
{
    pageControls[pageControlID] = {};
    pageControls[pageControlID]['nopages'] = true;
    pageControls[pageControlID]['control'] = $("#"        + pageControlID);
    pageControls[pageControlID]['header']  = $("#header_" + pageControlID);
    pageControls[pageControlID]['body']    = $("#body_"   + pageControlID);
}

function addCaption(pageControlID, tabID, caption, helpKeyword)
{

    var tab = $("#" + tabID);
    var helpElement = "";
    if(helpKeyword != 0)
    {
        helpElement =  "<img class='icon_button icon_help' onclick='showHelp(\"" + helpKeyword + "\");' src='/media/img/icons/help.png' />";
    }

    pageControls[pageControlID]['header'].append(
                "<div id='caption_" + tabID  + "' class='page_control_caption'>"
                + caption + helpElement
                + "</div>");
    $("#caption_" + tabID).click(function(){ changeTab(pageControlID, tabID); });
    if(pageControls[pageControlID]['nopages'])
    {
        changeTab(pageControlID, tabID);
        pageControls[pageControlID]['nopages'] = false;
    }
}

function changeTab(pageControlID, newTabID)
{
    $("#header_"  + pageControlID + " > div.page_control_caption").each(function() { $(this).removeClass("page_control_caption_selected"); });
    $("#body_"    + pageControlID + " > div.page_control_tab"    ).each(function() { $(this).removeClass("page_control_tab_selected"    ); });
    $("#"         + newTabID).addClass("page_control_tab_selected");
    $("#caption_" + newTabID).addClass("page_control_caption_selected");
}
