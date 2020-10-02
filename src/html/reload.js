function reload(url) {
    if (confirm("Do you want to reload data? (All unsaved data will be lost!)") == true) {
        window.location=url
    }
}