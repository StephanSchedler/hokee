function deleteBackup(url, file) 
{
    if (confirm('Do you want to delete backup file ' + file + '?') == true) 
    {
        window.location=url + '?file=' + file;
    } 
}