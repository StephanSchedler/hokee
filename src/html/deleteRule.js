function deleteRule(url, id) 
{
    if (confirm('Do you want to delete this rule?') == true) 
    {
        window.location=url + '?id=' + id;
    } 
}