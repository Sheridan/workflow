var storage = {};
function saveCookie(id, key, value)
{
    $.cookie(id+key, value, { path: '/' });
}

function loadCookie(id, key, defaultValue)
{
    var value = $.cookie(id+key);
    if(!value)
    {
        value = defaultValue;
        saveCookie(id, key, value);
    }
    return value;
}

function initLoader(id, data, query)
{
    storage[id] = {"container": {}, "pages": {}, "options": data, "query": query, 'init': true};
    storage[id]["container"]["base"] = $("#container_" + id);
    storage[id]["container"]["base"].addClass("container");
    storage[id]["container"]["base"].html
            (
                "<div class='header' id='header_" + id + "'></div>" +
                "<div class='body'   id='body_"   + id + "'></div>" +
                "<div class='footer' id='footer_" + id + "'></div>"
            );
    storage[id]["container"]["header"] = $("#header_" + id);
    storage[id]["container"]["body"]   = $("#body_"   + id);
    storage[id]["container"]["footer"] = $("#footer_" + id);


    storage[id]["container"]["header"].append
            (
                "<div class='page_count_controller' id='page_count_controller_" + id + "'>" +
                  "Строк на страницу:" +
                  "<img class='button' src='/media/img/icons/android-remove.svg' onclick='decrementItemsPerPage(\"" + id + "\");' />" +
                  "<div class='page_count_counter' id='items_per_page_counter_" + id + "'></div>" +
                  "<img class='button' src='/media/img/icons/android-add.svg'    onclick='incrementItemsPerPage(\"" + id + "\");' />" +
                "</div>"
            );
    storage[id]["container"]["items_per_page_counter"]   = $("#items_per_page_counter_" + id);
    storage[id]["container"]["footer"].append
            (
                "<div class='pages_walk_controller' id='page_walk_controller_" + id + "'>" +
                    "<img id='first_page_" + id + "' class='button' src='/media/img/icons/ios7-skipbackward.svg' onclick='goFirstPage(\""    + id + "\");' />" +
                    "<img id='prev_page_" + id + "' class='button' src='/media/img/icons/arrow-left-b.svg'      onclick='goPreviousPage(\"" + id + "\");' />" +
                    "<div class='pages_walk_pages' id='pages_walk_pages_" + id + "'></div>" +
                    "<img id='next_page_" + id + "' class='button' src='/media/img/icons/arrow-right-b.svg'     onclick='goNextPage(\"" + id + "\");' />" +
                    "<img id='last_page_" + id + "' class='button' src='/media/img/icons/ios7-skipforward.svg'  onclick='goLastPage(\"" + id + "\");' />" +
                "</div>"
            );
    storage[id]["container"]["pages_controller"]   = $("#page_walk_controller_" + id);
    storage[id]["container"]["pages_walk_pages"]   = $("#pages_walk_pages_" + id);

    storage[id]["container"]["first_page_button"] = $("#first_page_" + id);
    storage[id]["container"]["prev_page_button"]  = $("#prev_page_"  + id);
    storage[id]["container"]["next_page_button"]  = $("#next_page_"  + id);
    storage[id]["container"]["last_page_button"]  = $("#last_page_"  + id);

    storage[id]["container"]["pages_controller"].hide();

    setItemsPerPage(id, parseInt(loadCookie(id, 'items_per_page', 15)));
    goFirstPage(id);
    storage[id]['init'] = false;
    loadData(id);
}

// +++++++++++++++ Контроль количества строк на страницу +++++++++++++++++++
function setNavigateVisibility(id)
{
    if(storage[id]['pages']['total_pages'] > 1)
    {
        storage[id]["container"]["pages_controller"].show();
        if(storage[id]['pages']['current_page'] === 0)
        {
            storage[id]["container"]["first_page_button"].hide();
            storage[id]["container"]["prev_page_button"].hide();
        }
        else
        {
            storage[id]["container"]["first_page_button"].show();
            storage[id]["container"]["prev_page_button"].show();
        }
        if(storage[id]['pages']['current_page'] === storage[id]['pages']['total_pages']-1)
        {
            storage[id]["container"]["next_page_button"].hide();
            storage[id]["container"]["last_page_button"].hide();
        }
        else
        {
            storage[id]["container"]["next_page_button"].show();
            storage[id]["container"]["last_page_button"].show();
        }
    }
    else
    {
        storage[id]["container"]["pages_controller"].hide();
    }
}

function setItemsPerPage(id, newValue)
{
    storage[id]['pages']['items_per_page'] = newValue;
    saveCookie(id, 'items_per_page', storage[id]['pages']['items_per_page']);
    storage[id]["container"]["items_per_page_counter"].html(storage[id]['pages']['items_per_page']);
    if(!storage[id]['init']) { loadData(id); }
}

function decrementItemsPerPage(id)
{
    if((storage[id]['pages']['items_per_page'] - 5) > 0)
    {
        setItemsPerPage(id, storage[id]['pages']['items_per_page'] - 5);
    }
}

function incrementItemsPerPage(id)
{
    setItemsPerPage(id, storage[id]['pages']['items_per_page'] + 5);
}

// --------------- Контроль количества строк на страницу -------------------
// +++++++++++++++ Контроль смены страниц +++++++++++++++++++

function changePage(id, newPage)
{
    storage[id]['pages']['current_page'] = newPage;
    if(!storage[id]['init']) { loadData(id); }
}

function goFirstPage(id)
{
    changePage(id, 0);
}

function goPreviousPage(id)
{
    if((storage[id]['pages']['current_page'] - 1) >= 0)
    {
        changePage(id, storage[id]['pages']['current_page'] - 1);
    }
}

function goNextPage(id)
{
    if((storage[id]['pages']['current_page'] + 1) < storage[id]['pages']['total_pages'])
    {
        changePage(id, storage[id]['pages']['current_page'] + 1);
    }
}

function goLastPage(id)
{
    changePage(id, storage[id]['pages']['total_pages']-1);
}

function renderPages(id, resultRowCount)
{
    storage[id]['pages']['total_rows'] = resultRowCount;
    storage[id]['pages']['total_pages'] = Math.ceil(resultRowCount / storage[id]['pages']['items_per_page']);
    storage[id]["container"]["pages_walk_pages"].html("");
    for(var i=0; i<storage[id]['pages']['total_pages']; i++)
    {
        var subcalss = i == storage[id]['pages']['current_page'] ? "selected" : "";
        storage[id]["container"]["pages_walk_pages"].append("<div class='page " + subcalss + "' onclick='changePage(\"" + id + "\", " + i + ")'>" + Number(i+1) +"</div>");
    }
}

// --------------- Контроль смены страниц -------------------

// +++++++++++++++ Контроль загрузки +++++++++++++++++++

function loadData(id)
{
    var q = storage[id]['query'];
    q['itemsPerPage'] = storage[id]['pages']['items_per_page']
    q['offset']       = storage[id]['pages']['current_page']*storage[id]['pages']['items_per_page']
    $.post( storage[id]['options']['url'], q)
      .done(function( data )
      {
          if(data)
          {
              var jsonData = JSON.parse(data);
              renderPages(id, jsonData["totalRows"]);
              eval(storage[id]['options']['renderer'])(id, storage[id]["container"]["body"], jsonData['data']);
              setNavigateVisibility(id);
          }
      });
}

// --------------- Контроль загрузки -------------------
