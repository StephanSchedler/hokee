function submitMail(formId) {
    if (confirm('Do you want to send an email?') == true) {
        document.getElementById(formId).submit();
    }
}