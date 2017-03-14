function showJQUIDialog(id, options)
{
    $( "#dialog_"+id ).dialog(options);
}

function makeJQUIButton(id, icon)
{
    $( "#button_"+id ).button
        (
            {
              icons: { primary: icon },
              text: false
            }
        );
}

function makeJQUIDateEdit(id)
{
    $( "#"+id ).datepicker({
                              firstDay: 1,
                              showAnim: "slideDown"
                            });
    $( "#"+id ).datepicker.setDefaults( $.datepicker.regional[ "ru" ] );
}

function makeJQUIDateTimeEdit(id)
{
    $( "#"+id ).datetimepicker({
                              dayOfWeekStart: 1,
                              showAnim      : "slideDown",
                              lang          : "ru",
                              format        : 'd.m.Y H:i',
                              weekends      : [0, 6]
                            });
}

function showValueEditDialog(selectorUnique, functionNamePart, dbID)
{
    var dialog = $( "#dialog_" + selectorUnique + "_" + dbID + "_" + functionNamePart );
    dialog.dialog({
        modal: true,
        buttons:
        {
           "Отправить": function()
           {
               var options = eval("edit" + functionNamePart)(dialog, dbID);
               options['value']['comment'] = dialog.find("#comment_" + selectorUnique + "_" + dbID + "_" + functionNamePart).val();
               console.log(options);
               $.post(options['url'], options['value'],
                      function( data )
                      {
                          window.open(options['after'], '_self');
                      });

           },
           "Отмена"   : function() { $( this ).dialog( "close" );  }
        }
    });
}
