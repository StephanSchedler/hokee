#include "HtmlGenerator.h"
#include "InternalException.h"
#include "Settings.h"
#include "Utils.h"
#include "hokee.h"

#include <fmt/core.h>
#include <fmt/format.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>

namespace hokee
{
HtmlElement* HtmlGenerator::AddButton(HtmlElement* tableRow, const std::string& link, const std::string& tooltip,
                                      const std::string& image, const std::string& text, const std::string& style)
{
    auto cell = tableRow->AddTableCell();
    cell->SetAttribute("class", std::string("nav link ") + style);
    auto hyperlink = cell->AddHyperlink(link, tooltip);
    hyperlink->AddImage(image, tooltip, 42);
    hyperlink->AddText(text);
    return cell;
}

void HtmlGenerator::AddNavigationHeader(HtmlElement* body, const CsvDatabase& database)
{
    auto div = body->AddDivision();
    div->SetAttribute("class", "box nav");

    auto table = div->AddTable();
    table->SetAttribute("class", "nav");
    auto row = table->AddTableRow();

    AddButton(row, INDEX_HTML, "Show Summary", "48-file-excel.png", "Summary &nbsp;");
    AddButton(row, RULES_HTML, "Show Rules", "48-file-exe.png", fmt::format("Rules ({})", database.Rules.size()));
    AddButton(row, ALL_HTML, "Show All Items", "48-search.png",
              fmt::format("Items ({})", database.Data.size()));

    if (database.Unassigned.size() == 0 && database.Issues.size() == 0)
    {
        AddButton(row, ASSIGNED_HTML, "Show Assigned Items", "48-sign-check.png",
                  fmt::format("Assigned ({})", database.Assigned.size()));
    }
    else
    {
        AddButton(row, ASSIGNED_HTML, "Show Assigned Items", "48-sign-check.png",
                  fmt::format("Assigned ({})", database.Assigned.size()), "gray");
    }

    if (database.Unassigned.size() == 0)
    {
        AddButton(row, UNASSIGNED_HTML, "Show Unassigned Items", "48-sign-warning.png",
                  fmt::format("Warnings ({})", database.Unassigned.size()), "gray");
    }
    else
    {
        AddButton(row, UNASSIGNED_HTML, "Show Unassigned Items", "48-sign-warning.png",
                  fmt::format("Warnings ({})", database.Unassigned.size()));
    }

    if (database.Issues.size() == 0)
    {
        AddButton(row, ISSUES_HTML, "Show Issues", "48-sign-error.png",
                  fmt::format("Errors ({})", database.Issues.size()), "gray");
    }
    else
    {
        AddButton(row, ISSUES_HTML, "Show Issues", "48-sign-error.png",
                  fmt::format("Errors ({})", database.Issues.size()));
    }

    auto cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("class", "nav fill");

    cell = row->AddTableCell();
    cell->SetAttribute("class", "nav");
    cell->AddBold("hookee");
    cell->AddText(PROJECT_VERSION_SHORT);

    cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("class", "nav fill");

    AddButton(row, SETTINGS_HTML, "Open Settings File", "48-cogs.png", "Settings");
    AddButton(row, SUPPORT_HTML, "Generate Support Mail", "48-envelope-letter.png", "Support");
    AddButton(row, INPUT_CMD, "Open Input Folder", "48-folder.png", "Input");
    AddButton(row, HELP_HTML, "Open Online Help", "48-sign-question.png", "Help");
    auto reload = AddButton(row, "#", "Reload CSV Data", "48-sign-sync.png", "Reload");
    AddButton(row, EXIT_CMD, "Stop hokee", "48-sign-error.png", "Exit", "hue-200");

    reload->SetAttribute("onclick", fmt::format("reload('{}')", RELOAD_CMD));
    body->AddScript("reload.js");
}

HtmlElement* HtmlGenerator::AddHtmlHead(HtmlElement* html)
{
    auto head = html->AddHead();
    head->AddTitle("hokee");
    auto meta = head->AddMeta();
    meta->SetAttribute("name", "hokee");
    meta->SetAttribute("content", "Summary");
    meta = head->AddMeta();
    meta->SetAttribute("charset", "UTF-8");

    auto link = head->AddLink();
    link->SetAttribute("rel", "apple-touch-icon");
    link->SetAttribute("sizes", "180x180");
    link->SetAttribute("href", "/apple-touch-icon.png");

    link = head->AddLink();
    link->SetAttribute("rel", "icon");
    link->SetAttribute("type", "image/png");
    link->SetAttribute("sizes", "32x32");
    link->SetAttribute("href", "/favicon-32x32.png");

    link = head->AddLink();
    link->SetAttribute("rel", "icon");
    link->SetAttribute("type", "image/png");
    link->SetAttribute("sizes", "16x16");
    link->SetAttribute("href", "/favicon-16x16.png");

    link = head->AddLink();
    link->SetAttribute("rel", "mask-icon");
    link->SetAttribute("href", "/safari-pinned-tab.svg");
    link->SetAttribute("color", "#5bbad5");

    link = head->AddLink();
    link->SetAttribute("rel", "stylesheet");
    link->SetAttribute("href", "/stylesheet.css");

    return head;
}

void HtmlGenerator::AddItemTableHeader(HtmlElement* table)
{
    auto row = table->AddTableRow();
    row->AddTableHeaderCell("#");
    row->AddTableHeaderCell("Category");
    row->AddTableHeaderCell("Payer/Payee");
    row->AddTableHeaderCell("Description");
    row->AddTableHeaderCell("Type");
    row->AddTableHeaderCell("Date");
    row->AddTableHeaderCell("Account");
    row->AddTableHeaderCell("Value");
}

void HtmlGenerator::AddSummaryTableHeader(HtmlElement* table, const std::vector<std::string>& categories)
{
    auto row = table->AddTableRow();
    row->AddTableHeaderCell("Date");
    row->AddTableHeaderCell("*");

    for (size_t i = 1; i < categories.size(); ++i)
    {
        std::string cat = categories[i];
        if (!cat.empty() && cat.back() == '!')
        {
            // remove tailing "!"
            cat.pop_back();
        }
        row->AddTableHeaderCell(cat);
    }
}

std::string HtmlGenerator::GetSummaryPage(const CsvDatabase& database)
{
    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    AddNavigationHeader(body, database);

    auto main = body->AddMain();
    main->SetAttribute("class", "pad-100");
    main->AddHeading(2, "Summary");

    // Determine (used) Categories
    std::vector<std::string> categories{""};
    for (auto& rule : database.Rules)
    {
        if (std::find(categories.begin(), categories.end(), rule->Category) == categories.end())
        {
            categories.push_back(rule->Category);
        }
    }

    // Sort data
    int minYear = 3000;
    int maxYear = 1900;
    std::map<int, std::map<int, std::map<std::string, CsvTable>>> sortedTables;
    for (auto& item : database.Data)
    {
        const int year = item->Date.GetYear();
        const int month = item->Date.GetMonth();
        sortedTables[year][0][categories[0]].push_back(item);
        sortedTables[year][0][item->Category].push_back(item);
        sortedTables[year][month][categories[0]].push_back(item);
        sortedTables[year][month][item->Category].push_back(item);

        if (minYear > year)
        {
            minYear = year;
        }
        if (maxYear < year)
        {
            maxYear = year;
        }
    }

    auto table = main->AddTable();
    table->SetAttribute("class", "item mar-20");

    int rowCount = 0;
    for (int year = minYear; year <= maxYear; ++year)
    {
        AddSummaryTableHeader(table, categories);

        for (int month = 0; month <= 12; ++month)
        {
            rowCount++;
            auto row = table->AddTableRow();
            std::string name = fmt::format("{}.{}", month, year);
            if (month == 0)
            {
                row->SetAttribute("class", "year");
                name = fmt::format("{}", year);
            }

            auto cell = row->AddTableCell(name);
            cell->SetAttribute("class", "item");
            for (auto& cat : categories)
            {
                double sum = 0;
                for (auto& item : sortedTables[year][month][cat])
                {
                    if ((!item->Category.empty() && item->Category.back() == '!') && cat != item->Category)
                    {
                        continue;
                    }

                    try
                    {
                        sum += std::stod(item->Value);
                    }
                    catch (const std::exception& e)
                    {
                        throw InternalException(
                            __FILE__, __LINE__,
                            fmt::format("Could not convert '{}' to 'double'. ({})", item->Value, e.what()));
                    }
                }

                std::string cellStyle = "link";
                if (sum > 0)
                {
                    cellStyle += fmt::format(" pos pos-bg{}", rowCount % 2);
                }
                if (sum < 0)
                {
                    cellStyle += fmt::format(" neg neg-bg{}", rowCount % 2);
                }

                cell = row->AddTableCell();
                cell->SetAttribute("class", cellStyle);
                cell->SetAttribute("onclick",
                                   fmt::format("window.location='{}?year={}&amp;month={}&amp;category={}';",
                                               ITEMS_HTML, year, month, cat));
                cell->AddText(fmt::format("{:.2f}&euro;", sum));
            }
        }
    }
    return html.ToString();
}

std::string HtmlGenerator::GetTablePage(const CsvDatabase& database, const std::string& title,
                                        const CsvTable& data)
{
    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    AddNavigationHeader(body, database);

    auto main = body->AddMain();
    main->SetAttribute("class", "pad-100");

    auto table = main->AddTable();
    table->SetAttribute("class", "form");
    auto row = table->AddTableRow();
    row->SetAttribute("class", "form");
    auto cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    cell->SetAttribute("style", "width: 50%;");
    cell->AddHeading(2, title);
    cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    auto input = cell->AddInput();
    input->SetAttribute("id", "filter");
    input->SetAttribute("type", "text");
    input->SetAttribute("placeholder", "Filter...");
    input->SetAttribute("title", "Type in a string");
    input->SetAttribute("oninput", "filterTable('table', 'filter')");
    input->SetAttribute("class", "filter");

    table = main->AddTable();
    table->SetAttribute("id", "table");
    table->SetAttribute("class", "item mar-20");
    AddItemTableHeader(table);
    for (const auto& r : data)
    {
        AddItemTableRow(table, r.get());
    }
    body->AddScript("filterTable.js");

    return html.ToString();
}

std::string HtmlGenerator::GetErrorPage(int errorCode, const std::string& errorMessage)
{
    Utils::PrintError(fmt::format("HttpServer operation failed: {}", errorMessage));

    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    auto main = body->AddMain();
    main->SetAttribute("class", "mar-50");

    auto div = main->AddDivision();
    div->SetAttribute("class", "msg box red");

    auto p = div->AddDivision();
    p->AddHyperlinkImage(EXIT_CMD, "Stop hokee", "96-sign-ban.png", 96);

    div->AddHeading(2, fmt::format("ERROR {}", errorCode));
    div->AddParagraph()->AddBold(errorMessage);

    div->AddParagraph("&nbsp;");
    div->AddParagraph("&nbsp;");
    div->AddParagraph("What next?");

    auto table = div->AddDivision()->AddTable();
    table->SetAttribute("class", "nav");

    auto row = table->AddTableRow();

    auto cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("class", "nav fill");

    AddButton(row, INPUT_CMD, "Open Input Folder", "48-box-full.png", "Input Folder");
    AddButton(row, SUPPORT_HTML, "Generate Support Mail", "48-envelope-letter.png", "Get&nbsp;Support");
    AddButton(row, SETTINGS_HTML, "Open Settings File", "48-cogs.png", "Settings");
    auto reload = AddButton(row, "#", "Reload CSV Data", "48-sign-sync.png", "Reload");
    AddButton(row, EXIT_CMD, "Stop hokee", "48-sign-error.png", "Exit", "hue-200");

    reload->SetAttribute("onclick", "reload()");
    body->AddScript(std::string("function reload() {\n")
                    + "if (confirm(\"Do you want to reload data? (All unsaved data will be lost!)\") == true) {\n"
                    + fmt::format("    window.location='{}'\n", RELOAD_CMD) + "}\n" + "}");

    cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("class", "nav fill");

    return html.ToString();
}

std::string HtmlGenerator::GetHelpPage(const CsvDatabase& database)
{
    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    AddNavigationHeader(body, database);

    auto main = body->AddMain();
    main->SetAttribute("class", "pad-100");

    fs::path htmlPart = fs::current_path() / ".." / "html" / "help.html.part";
    main->AddText(Utils::ReadFileContent(htmlPart));

    return html.ToString();
}

std::string HtmlGenerator::GetSupportPage(const CsvDatabase& database, const fs::path& ruleSetFile,
                                          const fs::path& inputDir)
{
    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    AddNavigationHeader(body, database);

    auto main = body->AddMain();
    main->SetAttribute("class", "pad-100");

    auto form = main->AddForm();
    form->SetAttribute("action", "mailto:schedler@paderborn.com");
    form->SetAttribute("method", "post");
    form->SetAttribute("enctype", "text/plain");
    form->SetAttribute("id", "form");
    auto table = form->AddTable();
    table->SetAttribute("class", "form");
    auto row = table->AddTableRow();
    row->SetAttribute("class", "form");
    auto cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    cell->AddHeading(2, "Generate&nbsp;Support&nbsp;Mail");
    cell = row->AddTableCell();
    cell->SetAttribute("class", "form fill");
    cell = row->AddTableCell();
    cell->SetAttribute("class", "form");

    cell = row->AddTableCell();
    cell->AddImage("48-envelope-letter.png", "Submit", 40);
    cell->SetAttribute("class", "form link");
    cell->SetAttribute("onclick", "submitForm()");
    body->AddScript("function submitForm() {\n"
                    "  if (confirm(\"Do you want to send an email?\") == true) {\n"
                    "    document.getElementById(\"form\").submit();\n"
                    "  }\n"
                    "}");

    std::string mail = Utils::GenerateSupportMail(ruleSetFile, inputDir);
    auto label = form->AddLabel("Fill the section below and send it to schedler@paderborn.com");
    label->SetAttribute("class", "mar-20");
    auto textarea = label->AddTextarea(Utils::EscapeHtml(mail));
    textarea->SetAttribute("name", "body");
    textarea->SetAttribute("rows", "20");

    return html.ToString();
}

std::string HtmlGenerator::GetEditPage(const CsvDatabase& database, const fs::path& file)
{
    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    AddNavigationHeader(body, database);

    auto main = body->AddMain();
    main->SetAttribute("class", "pad-100");

    auto form = main->AddForm();
    std::string filename = file.string();
    form->SetAttribute("action", fmt::format("{}?file={}", HtmlGenerator::SAVE_CMD, filename));
    form->SetAttribute("method", "post");
    form->SetAttribute("enctype", "text/plain");
    form->SetAttribute("id", "form");

    auto table = form->AddTable();
    table->SetAttribute("class", "form");
    auto row = table->AddTableRow();
    row->SetAttribute("class", "form");
    auto cell = row->AddTableCell();
    cell->SetAttribute("class", "form fill");
    cell->AddHeading(2, "Edit&nbsp;File");
    cell = row->AddTableCell();
    cell->SetAttribute("class", "form");

    cell = row->AddTableCell();
    cell->AddImage("48-floppy.png", "Save File", 40);
    cell->SetAttribute("class", "form link");
    cell->SetAttribute("onclick", "submitForm()");
    body->AddScript("function submitForm() {\n"
                    "  if (confirm(\"Do you want to save changes?\") == true) {\n"
                    "    document.getElementById(\"form\").submit();\n"
                    "  }\n"
                    "}");

    table = form->AddTable();
    table->SetAttribute("class", "form");
    row = table->AddTableRow();
    row->SetAttribute("class", "form");
    cell = row->AddTableCell(file.string());
    cell->SetAttribute("class", "form fill mono center");

    cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    std::string link = fmt::format("{}?folder={}", HtmlGenerator::OPEN_CMD, file.parent_path().string());
    cell->AddHyperlinkImage(link, "Open folder", "48-folder.png", 38);

    table = form->AddTable();
    table->SetAttribute("class", "form");
    row = table->AddTableRow();
    row->SetAttribute("class", "form");

    std::string content = Utils::ReadFileContent(file);
    auto label = form->AddLabel("&nbsp;");
    auto textarea = label->AddTextarea(Utils::EscapeHtml(content));
    textarea->SetAttribute("name", "content");
    textarea->SetAttribute("rows", "20");

    return html.ToString();
}

void HtmlGenerator::AddInputForm(HtmlElement* table, const std::string& name, const std::string& value,
                                 const std::string& description)
{
    auto row = table->AddTableRow();
    auto cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    cell->SetAttribute("class", "form fill");
    auto label = cell->AddLabel(description);
    label->SetAttribute("class", "marb-20");
    auto input = label->AddInput();
    input->SetAttribute("class", "form");
    input->SetAttribute("type", "text");
    input->SetAttribute("name", name);
    input->SetAttribute("value", value);
}

std::string HtmlGenerator::GetSettingsPage(const CsvDatabase& database, const fs::path& file)
{
    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    AddNavigationHeader(body, database);

    auto main = body->AddMain();
    main->SetAttribute("class", "pad-100");

    auto form = main->AddForm();
    form->SetAttribute("action", HtmlGenerator::SETTINGS_HTML);
    form->SetAttribute("method", "get");
    form->SetAttribute("enctype", "text/plain");
    form->SetAttribute("id", "form");

    auto table = form->AddTable();
    table->SetAttribute("class", "form");
    auto row = table->AddTableRow();
    row->SetAttribute("class", "form");
    auto cell = row->AddTableCell();
    cell->SetAttribute("class", "form fill");
    cell->AddHeading(2, "Settings");
    cell = row->AddTableCell();
    cell->SetAttribute("class", "form");

    cell = row->AddTableCell();
    cell->AddImage("48-floppy.png", "Save Settings", 40);
    cell->SetAttribute("class", "form link");
    cell->SetAttribute("onclick", "submitForm()");
    body->AddScript(
        "function submitForm() {\n"
        "  if (confirm(\"Do you want to save settings? (You have to reload to apply new settings.)\") == true) {\n"
        "    document.getElementById(\"form\").submit();\n"
        "  }\n"
        "}");

    table = form->AddTable();
    table->SetAttribute("class", "form");
    row = table->AddTableRow();
    row->SetAttribute("class", "form");
    cell = row->AddTableCell(file.string());
    cell->SetAttribute("class", "form fill center mono");

    cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    std::string link = fmt::format("{}?file={}", HtmlGenerator::EDIT_HTML, file.string());
    cell->AddHyperlinkImage(link, "Edit Settings File", "48-notepad.png", 38);

    cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    link = fmt::format("{}?folder={}", HtmlGenerator::OPEN_CMD, file.parent_path().string());
    cell->AddHyperlinkImage(link, "Open folder", "48-folder.png", 38);

    table = form->AddTable();
    table->SetAttribute("class", "form");

    Settings config(file);
    AddInputForm(table, "InputDirectory", config.GetInputDirectory().string(), "Input directory*:");
    AddInputForm(table, "RuleSetFile", config.GetRuleSetFile().string(), "Rule definition file*:");
    AddInputForm(table, "Browser", config.GetBrowser(), "Webbrowser start command:");
    AddInputForm(table, "Explorer", config.GetExplorer(), "Fileexplorer start command:");
    AddInputForm(table, "Editor", config.GetEditor(), "Editor start command:");

    main->AddParagraph(fmt::format("*Paths can be absolute or relative to \"{}\"", file.parent_path().string()));

    return html.ToString();
}

std::string HtmlGenerator::GetEmptyInputPage()
{
    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    auto main = body->AddMain();
    main->SetAttribute("class", "mar-50");

    auto div = main->AddDivision();
    div->SetAttribute("class", "msg box");

    auto p = div->AddParagraph();
    p->AddHyperlinkImage(INPUT_CMD, "Open Input Folder", "96-box.png", 96);

    div->AddHeading(2, "Could not find any input data!");

    div->AddParagraph("&nbsp;");
    div->AddParagraph("&nbsp;");
    div->AddParagraph("What next?");
    auto table = div->AddDivision()->AddTable();
    table->SetAttribute("class", "nav");

    auto row = table->AddTableRow();

    auto cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("class", "nav fill");

    AddButton(row, COPY_SAMPLES_CMD, "Copy Samples", "48-box-in.png", "Copy&nbsp;Samples");
    AddButton(row, SETTINGS_HTML, "Open Settings", "48-cogs.png", "Open&nbsp;Settings");
    auto reload = AddButton(row, "#", "Reload CSV Data", "48-sign-sync.png", "Reload");
    AddButton(row, EXIT_CMD, "Stop hokee", "48-sign-error.png", "Exit", "hue-200");

    reload->SetAttribute("onclick", "reload()");
    body->AddScript(
        std::string("function reload() {\n")
        + "  if (confirm(\"Do you want to reload data? (All unsaved data will be lost!)\") == true) {\n"
        + fmt::format("    window.location='{}'\n", RELOAD_CMD) + "  }\n" + "}");

    cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("class", "nav fill");

    return html.ToString();
}

std::string HtmlGenerator::GetProgressPage(size_t value, size_t max)
{
    const std::vector<std::string> lastMessages = Utils::GetLastMessages();
    const std::string m0 = lastMessages.size() > 0 ? lastMessages[0] : "";
    const std::string m1 = lastMessages.size() > 1 ? lastMessages[1] : "";
    const std::string m2 = lastMessages.size() > 2 ? lastMessages[2] : "";
    const std::string m3 = lastMessages.size() > 3 ? lastMessages[3] : "";
    const std::string m4 = lastMessages.size() > 4 ? lastMessages[4] : "";
    const std::string m5 = lastMessages.size() > 5 ? lastMessages[5] : "";
    const std::string m6 = lastMessages.size() > 6 ? lastMessages[6] : "";
    const std::string m7 = lastMessages.size() > 7 ? lastMessages[7] : "";

    HtmlElement html;
    auto head = AddHtmlHead(&html);
    auto meta = head->AddMeta();
    meta->SetAttribute("http-equiv", "refresh");
    meta->SetAttribute("content", "1");

    auto body = html.AddBody();
    auto main = body->AddMain();
    main->SetAttribute("class", "mar-50");

    auto div = main->AddDivision();
    div->SetAttribute("class", "msg box");
    div->AddParagraph("&nbsp;");
    div->AddDivision(m7)->SetAttribute("style", "opacity:0.1;");
    div->AddDivision(m6)->SetAttribute("style", "opacity:0.2;");
    div->AddDivision(m5)->SetAttribute("style", "opacity:0.3;");
    div->AddDivision(m4)->SetAttribute("style", "opacity:0.4;");
    div->AddDivision(m3)->SetAttribute("style", "opacity:0.6;");
    div->AddDivision(m2)->SetAttribute("style", "opacity:0.8;");
    div->AddDivision(m1)->SetAttribute("style", "opacity:1;");
    div->AddParagraph("&nbsp;");
    div->AddDivision()->AddProgress(value, max);
    return html.ToString();
}

std::string HtmlGenerator::GetItemPage(const CsvDatabase& database, int id)
{
    std::string title = "";
    bool isItem = false;
    std::shared_ptr<CsvItem> item = nullptr;
    for (auto& i : database.Data)
    {
        if (i->Id == id)
        {
            item = i;
            title = fmt::format("Item&nbsp;{}", id);
            isItem = true;
            break;
        }
    }
    if (!item)
    {
        for (auto& r : database.Rules)
        {
            if (r->Id == id)
            {
                item = r;
                title = fmt::format("Rule&nbsp;{}", id);
                break;
            }
        }

        if (!item)
        {
            throw UserException(fmt::format("Could not find item: {}", id));
        }
    }

    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    AddNavigationHeader(body, database);

    auto main = body->AddMain();
    main->SetAttribute("class", "pad-100");

    auto table = main->AddTable();
    table->SetAttribute("class", "form");
    auto row = table->AddTableRow();
    row->SetAttribute("class", "form");

    if (!isItem)
    {
        auto cell = row->AddTableCell();
        cell->AddImage("48-sign-delete.png", "Delete this rule", 38);
        cell->SetAttribute("class", "form link");
        cell->SetAttribute("onclick", "deleteRule()");
        body->AddScript(std::string("function deleteRule() {\n"
                                    "  if (confirm(\"Do you want to delete this rule?\") == true) {\n")
                        + fmt::format("    window.location='{}?id={}';\n", DELETE_CMD, id) + "  }\n" + "}");
    }

    auto cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    cell->AddHeading(2, title);

    cell = row->AddTableCell();
    cell->SetAttribute("class", "form fill");

    if (!isItem)
    {
        cell = row->AddTableCell();
        cell->AddImage("48-floppy.png", "Save Rules", 40);
        cell->SetAttribute("class", "form link");
        cell->SetAttribute("onclick", "saveRules()");
        body->AddScript(std::string("function saveRules() {\n")
                        + "  if (confirm(\"Do you want to save all rules? (You have to reload to apply new "
                          "rules.)\") == true) {\n"
                        + fmt::format("   window.location='{}'\n", SAVE_CMD) + "  }\n" + "}");
    }

    if (database.Unassigned.HasItem(id))
    {
        int prevId = database.Unassigned.PrevItem(id);
        if (prevId >= 0)
        {
            cell = row->AddTableCell();
            cell->SetAttribute("class", "form hue-220");
            std::string link = fmt::format("{}?id={}", HtmlGenerator::ITEM_HTML, prevId);
            cell->AddHyperlinkImage(link, "Previous Warning", "48-sign-left.png", 38);
        }
        int nextId = database.Unassigned.NextItem(id);
        if (nextId >= 0)
        {
            cell = row->AddTableCell();
            cell->SetAttribute("class", "form hue-220");
            std::string link = fmt::format("{}?id={}", HtmlGenerator::ITEM_HTML, nextId);
            cell->AddHyperlinkImage(link, "Next Warning", "48-sign-right.png", 38);
        }
        else
        {
            cell = row->AddTableCell();
            cell->SetAttribute("class", "form");
            auto div = cell->AddDivision();
            div->SetAttribute("style", "width: 38px;");
        }
    }
    if (database.Issues.HasItem(id))
    {
        int prevId = database.Issues.PrevItem(id);
        if (prevId >= 0)
        {
            cell = row->AddTableCell();
            cell->SetAttribute("class", "form hue-180");
            std::string link = fmt::format("{}?id={}", HtmlGenerator::ITEM_HTML, prevId);
            cell->AddHyperlinkImage(link, "Previous Error", "48-sign-left.png", 38);
        }
        int nextId = database.Issues.NextItem(id);
        if (nextId >= 0)
        {
            cell = row->AddTableCell();
            cell->SetAttribute("class", "form hue-180");
            std::string link = fmt::format("{}?id={}", HtmlGenerator::ITEM_HTML, nextId);
            cell->AddHyperlinkImage(link, "Next Error", "48-sign-right.png", 38);
        }
        else
        {
            cell = row->AddTableCell();
            cell->SetAttribute("class", "form");
            auto div = cell->AddDivision();
            div->SetAttribute("style", "width: 38px;");
        }
    }

    table = main->AddTable();
    table->SetAttribute("class", "form");
    row = table->AddTableRow();
    row->SetAttribute("class", "form");
    cell = row->AddTableCell();

    cell = row->AddTableCell(fmt::format("{}:{}", item->File.string(), item->Line));
    cell->SetAttribute("class", "form mono fill center");

    cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    std::string link = fmt::format("{}?file={}", HtmlGenerator::EDIT_HTML, item->File.string());
    cell->AddHyperlinkImage(link, "Open folder", "48-notepad.png", 38);

    cell = row->AddTableCell();
    cell->SetAttribute("class", "form");
    link = fmt::format("{}?folder={}", HtmlGenerator::OPEN_CMD, item->File.parent_path().string());
    cell->AddHyperlinkImage(link, "Open folder", "48-folder.png", 38);

    fs::path formatFile = item->File.parent_path() / "format.ini";
    if (Utils::ToLower(item->File.extension().string()) == ".csv" && fs::exists(formatFile))
    {
        cell = row->AddTableCell();
        cell->SetAttribute("class", "form");
        link = fmt::format("{}?file={}", HtmlGenerator::EDIT_HTML, formatFile.string());
        cell->AddHyperlinkImage(link, "Open corresponding format file", "48-wrench-screwdriver.png", 38);
    }

    auto div = main->AddDivision();
    table = div->AddTable();
    table->SetAttribute("class", "item mar-20");
    AddItemTableHeader(table);
    AddItemTableRow(table, item.get());

    if (!item->Issues.empty() || item->References.empty())
    {
        main->AddHeading(3, "Issues:");
        table = main->AddTable();
        table->SetAttribute("class", "err mar-20");

        if (item->References.empty())
        {
            row = table->AddTableRow();
            cell = row->AddTableCell();
            cell->AddImage("48-sign-warning.png", "warning", 20);

            if (isItem)
            {
                cell = row->AddTableCell("WARNING: Item has no matching rule");
            }
            else
            {
                cell = row->AddTableCell("WARNING: Rule has no matching item(s)");
            }
            cell->SetAttribute("class", "warn fill");
        }

        for (auto& issue : item->Issues)
        {
            row = table->AddTableRow();
            cell = row->AddTableCell();
            cell->AddImage("48-sign-error.png", "error", 20);
            cell = row->AddTableCell(issue);
            cell->SetAttribute("class", "err fill");
        }
    }

    table = main->AddTable();
    table->SetAttribute("class", "form mar-20");
    row = table->AddTableRow();
    row->SetAttribute("class", "form");
    cell = row->AddTableCell();
    if (isItem)
    {
        cell->SetAttribute("class", "form link");
        cell->AddImage("48-sign-add.png", "Add new rule", 38);
        cell->SetAttribute("onclick", "");

        cell = row->AddTableCell();
        cell->SetAttribute("class", "form");
        cell->AddHeading(3, "Rule(s):");

        cell = row->AddTableCell();
        cell->SetAttribute("class", "form fill");
    }
    else
    {
        cell->SetAttribute("class", "form fill");
        cell->AddHeading(3, "Item(s):");
    }

    table = main->AddDivision()->AddTable();
    table->SetAttribute("class", "item mar-20");
    AddItemTableHeader(table);

    for (auto& ref : item->References)
    {
        AddItemTableRow(table, ref);
    }

    return html.ToString();
}

void HtmlGenerator::AddItemTableRow(HtmlElement* table, CsvItem* row)
{
    std::string rowStyle = "link";
    if (row->References.size() == 0)
    {
        rowStyle += " unassigned";
    }
    if (row->Issues.size() > 0)
    {
        rowStyle += " issue";
    }

    double value;
    std::string colorStyle{};
    try
    {
        value = std::stod(row->Value);
        if (value < 0)
        {
            colorStyle = "neg";
        }
        else if (value > 0)
        {
            colorStyle = "pos";
        }
    }
    catch (const std::exception&)
    {
        colorStyle = "";
    }

    auto htmlRow = table->AddTableRow();
    htmlRow->SetAttribute("class", rowStyle);
    htmlRow->SetAttribute("onclick", fmt::format("window.location='{}?id={}';", ITEM_HTML, row->Id));

    auto cell = htmlRow->AddTableCell(fmt::format("{}", row->Id));
    htmlRow->AddTableCell(row->Category.empty() ? "&nbsp;" : row->Category);
    htmlRow->AddTableCell(row->PayerPayee.empty() ? "&nbsp;" : row->PayerPayee);
    htmlRow->AddTableCell(row->Description.empty() ? "&nbsp;" : row->Description);
    htmlRow->AddTableCell(row->Type.empty() ? "&nbsp;" : row->Type);
    htmlRow->AddTableCell(row->Date.ToString().empty() ? "&nbsp;" : row->Date.ToString());
    htmlRow->AddTableCell(row->Account.empty() ? "&nbsp;" : row->Account);
    cell = htmlRow->AddTableCell(row->Value.empty() ? "&nbsp;" : row->Value);
    cell->SetAttribute("class", colorStyle);
}

} // namespace hokee