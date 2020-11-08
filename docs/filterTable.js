function filterTable(tableId, filterId)
{
    var input, filter, table, tr, match, td, i, j, k, txtValue;
    input = document.getElementById(filterId);
    filter = input.value.toUpperCase().split(' ');
    table = document.getElementById(tableId);
    tr = table.getElementsByTagName('tr');
    for (i = 1; i < tr.length; i++)
    {
        match = 0;
        for (k = 0; k < filter.length; k++)
        {
            for (j = 0; j < table.rows[0].cells.length; j++)
            {
                td = tr[i].getElementsByTagName('td')[j];
                if (td)
                {
                    txtValue = td.textContent || td.innerText;
                    if (txtValue.toUpperCase().indexOf(filter[k]) > -1)
                    {
                        match++;
                        break;
                    }
                }
            }
        }
        if (match == filter.length)
        {
            tr[i].style.display = '';
        }
        else
        {
            tr[i].style.display = 'none';
        }
    }
}