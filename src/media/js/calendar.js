function showDayDetails(url, what)
{
    initLoader('calendar_day_details',
      {
        'url'     : url,
        'renderer': 'taskListRenderer'
      });
    $.post(url)
      .done(function( data )
      {
          var dialog = $( "#");
          dialog.html(data);
          showJQUIDialog('help', {buttons: {"Закрыть"   : function() { $( this ).dialog( "close" );  }}});
      });
}
