function filterSummary(tableId, filter)
{
    var table, tr, i;
    table = document.getElementById(tableId);
    tr = table.getElementsByTagName('tr');
    
    for (i = 0; i < tr.length; i++)
    {
        if (i % 40 == 0 || filter == tr[i].title)
        {
            tr[i].style.display = '';
        }
        else
        {
            tr[i].style.display = 'none';
        }
    }
}

function scrollSummary() 
{
    let div = document.getElementsByClassName("tab")[0];
    div.scrollLeft = div.scrollWidth;
}