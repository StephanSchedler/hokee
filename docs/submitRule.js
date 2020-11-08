function submitRule(formId)
{
    var select = document.getElementById("Category"); 
    if (select.value == "NEW CATEGORY...")
    {
        var cat = prompt("Enter a new category : ", "category");
        if (cat == null || cat == "")
        {
            return;
        }

        var option = document.createElement("option");
        option.text = cat;
        option.value = cat;
        select.add(option);
        select.value = cat;
    }
    else if (select.value == "NEW IGNORE CATEGORY...")
    {
        var cat = prompt("Enter a new ignore category : ", "ignore");
        if (cat == null || cat == "")
        {
            return;
        }
        cat = cat + "!";
        
        var option = document.createElement("option");
        option.text = cat;
        option.value = cat;
        select.add(option);
        select.value = cat;
    }
    document.getElementById(formId).submit();
}

function insertSelectedText(event)
{
    if (event.which == 3) 
    {
        event.currentTarget.value = event.currentTarget.value.substring(event.currentTarget.selectionStart,
            event.currentTarget.selectionEnd);
    }
}