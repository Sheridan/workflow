function showHelp(keyword)
{
    $.get("/help/" + keyword)
      .done(function( data )
      {
          var dialog = $( "#dialog_help");
          dialog.html(data);
          showJQUIDialog('help', {buttons: {"Закрыть"   : function() { $( this ).dialog( "close" );  }}});
      });
}
