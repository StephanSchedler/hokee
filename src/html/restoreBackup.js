function restoreBackup(url, file) 
{
    if (confirm('Do you want to restore backup file ' + file + '?') == true) 
    {
        window.location=url + '?file=' + file;
    } 
}