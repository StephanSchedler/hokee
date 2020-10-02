function saveRules(url) 
{
    if (confirm('Do you want to save all rules? (You have to reload to apply changed rules.)') == true) 
    {
        window.location=url;
    }
}