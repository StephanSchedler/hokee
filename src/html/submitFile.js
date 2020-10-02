function submitFile(formId)
{
    if (confirm('Do you want to save changes?') == true) 
    {
        document.getElementById(formId).submit();
    }
}