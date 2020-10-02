function submitSettings(formId)
{
    if (confirm('Do you want to save settings? (You have to reload to apply changed settings.)') == true) 
    {
        document.getElementById(formId).submit();
    }
}