function deleteRule(url, id) 
{
    if (confirm('Do you want to delete this rule? (You have to reload to apply changed rules.)') == true) 
    {
        window.location=url + '?id=' + id;
    } 
}