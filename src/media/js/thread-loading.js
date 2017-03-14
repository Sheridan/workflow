function threadLoad(url, targetID)
{
    var targetElement = $( "#"+targetID );
    targetElement.append("<div id='l_" + targetID + "_l'><div class='loading'><img src='/media/img/loading/16x16.gif' alt='Loading...' /></div></div> ");
    $.get(url, function(data) { targetElement.html(data); $( "#l_" + targetID + "_l").remove(); })
}

