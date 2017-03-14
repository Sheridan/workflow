function changeListType(url, listType, element)
{
    $.post("/file/chtype/" + listType)
      .done(function( data )
      {
          threadLoad(url, $(element).parent().parent().attr('id'));
      });
}
