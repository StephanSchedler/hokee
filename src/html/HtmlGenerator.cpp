#include "HtmlGenerator.h"
#include "InternalException.h"
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
void HtmlGenerator::AddButton(HtmlElement* tableRow, const std::string& link, const std::string& tooltip,
                              const std::string& image, const std::string& text)
{
    auto cell = tableRow->AddTableCell();
    cell->SetAttribute("style", "border: hidden; text-align:center;");
    auto hyperlink = cell->AddHyperlink(link, tooltip);
    hyperlink->AddImage(image, tooltip, 42);
    hyperlink->AddText(text);
}

void HtmlGenerator::AddNavigationHeader(HtmlElement* body, const CsvDatabase& database)
{
    auto div = body->AddDivision();
    div->SetAttribute("style", "border-bottom: 1px solid black; position: fixed; "
                               "background: #E0E0E0;"
                               "border: 1px solid #DDD;"
                               "box-shadow: 5px 5px 5px rgba(0,0,0, .2);");
    auto table = div->AddTable();
    table->SetAttribute("style", "border-collapse: collapse; border: hidden; box-shadow: none;");
    auto row = table->AddTableRow();

    AddButton(row, INDEX_HTML, "Show Summary", "48-file-excel.png", "Summary &nbsp;");
    AddButton(row, RULES_HTML, "Show Rules", "48-file-exe.png", fmt::format("Rules ({})", database.Rules.size()));
    AddButton(row, ALL_HTML, "Show All Items", "48-file-text.png",
              fmt::format("Items ({})", database.Data.size()));

    if (database.Unassigned.size() == 0 && database.Issues.size() == 0)
    {
        AddButton(row, ASSIGNED_HTML, "Show Assigned Items", "48-sign-check.png",
                  fmt::format("Assigned ({})", database.Assigned.size()));
    }
    else
    {
        AddButton(row, ASSIGNED_HTML, "Show Assigned Items", "48-sign-check2.png",
                  fmt::format("Assigned ({})", database.Assigned.size()));
    }

    if (database.Unassigned.size() == 0)
    {
        AddButton(row, UNASSIGNED_HTML, "Show Unassigned Items", "48-sign-delete2.png",
                  fmt::format("Unassigned ({})", database.Unassigned.size()));
    }
    else
    {
        AddButton(row, UNASSIGNED_HTML, "Show Unassigned Items", "48-sign-delete.png",
                  fmt::format("Unassigned ({})", database.Unassigned.size()));
    }

    if (database.Issues.size() == 0)
    {
        AddButton(row, ISSUES_HTML, "Show Issues", "48-sign-warning2.png",
                  fmt::format("Issues ({})", database.Issues.size()));
    }
    else
    {
        AddButton(row, ISSUES_HTML, "Show Issues", "48-sign-warning.png",
                  fmt::format("Issues ({})", database.Issues.size()));
    }

    auto cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("style", "border: hidden; width:30%;");

    cell = row->AddTableCell();
    cell->SetAttribute("style", "border: hidden; text-align:center;");
    cell->AddBold("hookee");
    cell->AddText(PROJECT_VERSION_SHORT);

    cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("style", "border: hidden; width:30%;");

    AddButton(row, RELOAD_CMD, "Reload CSV Data", "48-sign-sync.png", "Reload");
    AddButton(row, SEARCH_HTML, "Open Search Page", "48-search.png", "Search");
    AddButton(row, INPUT_CMD, "Open Input Folder", "48-box-full.png", "Input");
    AddButton(row, SETTINGS_CMD, "Open Settings File", "48-cogs.png", "Settings");
    AddButton(row, SUPPORT_CMD, "Generate Support Mail", "48-envelope-letter.png", "Support");
    AddButton(row, HELP_HTML, "Open Online Help", "48-sign-question.png", "Help");
    AddButton(row, EXIT_CMD, "Stop hokee", "48-sign-error.png", "Exit");
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

    head->AddStyle("#left {width: 10%; display: inline-block; text-align:left;}\n"
                   "#middle {width: 80%; display: inline-block; text-align:center;}\n"
                   "#right {width: 10%; display: inline-block; text-align:right;}\n"
                   "a:link {\n"
                   "    text-decoration: none;\n"
                   "    color: inherit;\n"
                   "}\n"
                   "a:visited {\n"
                   "    text-decoration: none;\n"
                   "    color: inherit;\n"
                   "}\n"
                   "a:hover {\n"
                   "    text-decoration: underline;\n"
                   "    color: inherit;\n"
                   "}\n"
                   "a:active {\n"
                   "    text-decoration: none;\n"
                   "    color: inherit;\n"
                   "}\n"
                   "table {\n"
                   "  width:100%;\n"
                   "  box-shadow: 5px 5px 5px rgba(0,0,0, .2);\n"
                   "}\n"
                   "table, th, td {\n"
                   "  border: 1px solid #333;\n"
                   "  border-collapse: collapse;\n"
                   "}\n"
                   "th, td {\n"
                   "  padding: 5px;\n"
                   "  text-align: left;\n"
                   "}\n"
                   "table#t01 tr:nth-child(even) {\n"
                   "  background-color: #E0E0E0;\n"
                   "}\n"
                   "table#t01 tr:nth-child(odd) {\n"
                   "  background-color: #F7F7F7;\n"
                   "}\n"
                   "table#t01 th {\n"
                   "  background-color: #333;\n"
                   "  color: white;\n"
                   "}\n");
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
    main->SetAttribute("style", "padding-top: 100px;");
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
    table->SetAttribute("id", "t01");

    for (int year = minYear; year <= maxYear; ++year)
    {
        AddSummaryTableHeader(table, categories);

        for (int month = 0; month <= 12; ++month)
        {
            std::string rowStyle = "";
            std::string name = fmt::format("{}.{}", month, year);
            if (month == 0)
            {
                name = fmt::format("{}", year);
                rowStyle = "background-color:#999; font-weight: bold";
            }

            auto row = table->AddTableRow();
            row->SetAttribute("style", rowStyle);
            row->AddTableCell(name);
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

                std::string cellStyle = "";
                if (sum > 0)
                {
                    cellStyle = "color: #007F00;";
                    if (month == 0)
                    {
                        cellStyle += " background-color:#9C9; font-weight: bold;";
                    }
                    else
                    {
                        cellStyle += " background-color:#CFC;";
                    }
                }
                if (sum < 0)
                {
                    cellStyle = "color: #BF0000;";
                    if (month == 0)
                    {
                        cellStyle += " background-color:#C99; font-weight: bold;";
                    }
                    else
                    {
                        cellStyle += " background-color:#FCC;";
                    }
                }

                auto cell = row->AddTableCell();
                cellStyle += " cursor: pointer;";
                cell->SetAttribute("style", cellStyle);
                cell->SetAttribute("onclick", fmt::format("window.location='{}?year={}&amp;month={}&amp;category={}';", ITEMS_HTML, year, month, cat));
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
    main->SetAttribute("style", "padding-top: 100px;");
    main->AddHeading(2, title);

    auto table = main->AddTable();
    table->SetAttribute("id", "t01");
    AddItemTableHeader(table);
    for (const auto& row : data)
    {
        AddItemTableRow(table, row.get());
    }

    return html.ToString();
}

std::string HtmlGenerator::GetErrorPage(int errorCode, const std::string& errorMessage)
{
    Utils::PrintError(fmt::format("HttpServer operation failed: {}", errorMessage));

    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    auto main = body->AddMain();
    main->SetAttribute("style", "padding: 50px 0;");

    auto div = main->AddDivision();
    div->SetAttribute("style", "text-align:center;\n"
                               "color: red;\n"
                               "margin: auto;\n"
                               "width: 80%;\n"
                               "padding: 20px;\n"
                               "background: #F0F0F0;\n"
                               "border: 1px solid #DDD;\n"
                               "box-shadow: 5px 5px 5px rgba(0,0,0, .2);");

    auto p = div->AddDivision();
    p->AddHyperlinkImage(EXIT_CMD, "Stop hokee", "96-sign-ban.png", 96);

    div->AddHeading(2, fmt::format("ERROR {}", errorCode));
    div->AddParagraph()->AddBold(errorMessage);

    div->AddParagraph("&nbsp;");
    div->AddParagraph("&nbsp;");
    div->AddParagraph("What next?")->SetAttribute("style", "color: #000000;");
    auto table = div->AddDivision()->AddTable();
    table->SetAttribute("style", "border-collapse: collapse; border: hidden; box-shadow: none;");

    auto row = table->AddTableRow();

    auto cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("style", "border: hidden; width:50%;");

    AddButton(row, RELOAD_CMD, "Reload", "48-sign-sync.png", "Reload");
    AddButton(row, INPUT_CMD, "Open Input Folder", "48-box-full.png", "Input");
    AddButton(row, SUPPORT_CMD, "Generate Support Mail", "48-envelope-letter.png", "Get&nbsp;Support");
    AddButton(row, SETTINGS_CMD, "Open Settings File", "48-cogs.png", "Settings");
    AddButton(row, EXIT_CMD, "Stop hokee", "48-sign-error.png", "Exit");

    cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("style", "border: hidden; width:50%;");

    return html.ToString();
}

std::string HtmlGenerator::GetHelpPage(const CsvDatabase& database)
{
    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    AddNavigationHeader(body, database);

    auto main = body->AddMain();
    main->SetAttribute("style", "padding-top: 100px;");

    fs::path htmlPart = fs::current_path() / ".." / "html" / "help.html.part";

    std::ifstream ifstream(htmlPart, std::ios::binary);
    std::stringstream sstream{};
    sstream << ifstream.rdbuf();
    main->AddText(sstream.str());

    return html.ToString();
}

std::string HtmlGenerator::GetEmptyInputPage()
{
    HtmlElement html;
    AddHtmlHead(&html);

    auto body = html.AddBody();
    auto main = body->AddMain();
    main->SetAttribute("style", "padding: 50px 0;");

    auto div = main->AddDivision();
    div->SetAttribute("style", "text-align:center;\n"
                               "margin: auto;\n"
                               "width: 80%;\n"
                               "padding: 20px;\n"
                               "background: #F0F0F0;\n"
                               "border: 1px solid #DDD;\n"
                               "box-shadow: 5px 5px 5px rgba(0,0,0, .2);");

    auto p = div->AddParagraph();
    p->AddHyperlinkImage(INPUT_CMD, "Open Input Folder", "96-box.png", 96);

    div->AddHeading(2, "Could not find any input data!");

    div->AddParagraph("&nbsp;");
    div->AddParagraph("&nbsp;");
    div->AddParagraph("What next?")->SetAttribute("style", "color: #000000;");
    auto table = div->AddDivision()->AddTable();
    table->SetAttribute("style", "border-collapse: collapse; border: hidden; box-shadow: none;");

    auto row = table->AddTableRow();

    auto cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("style", "border: hidden; width:50%;");

    AddButton(row, COPY_SAMPLES_CMD, "Copy Samples", "48-box-in.png", "Copy&nbsp;Samples");
    AddButton(row, RELOAD_CMD, "Reload", "48-sign-sync.png", "Reload");
    AddButton(row, SETTINGS_CMD, "Open Settings", "48-cogs.png", "Open&nbsp;Settings");
    AddButton(row, EXIT_CMD, "Stop hokee", "48-sign-error.png", "Exit");

    cell = row->AddTableCell("&nbsp;");
    cell->SetAttribute("style", "border: hidden; width:50%;");

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
    main->SetAttribute("style", "padding: 50px 0;");

    auto div = main->AddDivision();
    div->SetAttribute("style", "text-align:center;\n"
                               "margin: auto;\n"
                               "width: 80%;\n"
                               "padding: 20px;\n"
                               "background: #F0F0F0;\n"
                               "border: 1px solid #DDD;\n"
                               "box-shadow: 5px 5px 5px rgba(0,0,0, .2);");
    div->AddParagraph("&nbsp;");
    div->AddDivision(m7)->SetAttribute("style", "color: #E0E0E0;");
    div->AddDivision(m6)->SetAttribute("style", "color: #C0C0C0;");
    div->AddDivision(m5)->SetAttribute("style", "color: #A0A0A0;");
    div->AddDivision(m4)->SetAttribute("style", "color: #808080;");
    div->AddDivision(m3)->SetAttribute("style", "color: #404040;");
    div->AddDivision(m2)->SetAttribute("style", "color: #202020;");
    div->AddDivision(m1)->SetAttribute("style", "color: #000000;");
    div->AddParagraph("&nbsp;");
    div->AddDivision()->AddProgress(value, max);
    return html.ToString();
}

void HtmlGenerator::AddItemHyperlink(HtmlElement* cell, int id)
{
    cell->AddHyperlink(fmt::format("{}?id={}", HtmlGenerator::ITEM_HTML, id), "Open item",
                       fmt::format("{:#04}", id));
}

void HtmlGenerator::AddEditorHyperlink(HtmlElement* element, const fs::path& file)
{
    if (fs::is_directory(file))
    {
        std::string ref = file.string();
        replace(ref.begin(), ref.end(), '\\', '/');
        std::string link = fmt::format("{}?folder={}", HtmlGenerator::OPEN_CMD, ref);
        element->AddHyperlinkImage(link, "Open folder", "24-folder.png", 24);
    }
    else
    {
        std::string ref = file.string();
        replace(ref.begin(), ref.end(), '\\', '/');
        std::string link = fmt::format("{}?file={}", HtmlGenerator::EDIT_CMD, ref);
        element->AddHyperlinkImage(link, "Open in editor", "24-notepad.png", 24);

        ref = file.parent_path().string();
        replace(ref.begin(), ref.end(), '\\', '/');
        link = fmt::format("{}?folder={}", HtmlGenerator::OPEN_CMD, ref);
        element->AddHyperlinkImage(link, "Open parent folder", "24-folder.png", 24);

        fs::path formatFile = file.parent_path() / "format.ini";
        if (Utils::ToLower(file.extension().string()) == ".csv" && fs::exists(formatFile))
        {
            ref = formatFile.string();
            replace(ref.begin(), ref.end(), '\\', '/');
            link = fmt::format("{}?file={}", HtmlGenerator::EDIT_CMD, ref);
            element->AddHyperlinkImage(link, "Open corresponding format file", "24-wrench-screwdriver.png", 24);
        }
    }
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
            title = fmt::format("Item {}", id);
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
                title = fmt::format("Rule {}", id);
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
    main->SetAttribute("style", "padding-top: 100px;");
    auto h2 = main->AddHeading(2, title);
    AddEditorHyperlink(h2, item->File);

    auto table = main->AddDivision()->AddTable();
    table->SetAttribute("id", "t01");
    AddItemTableHeader(table);
    AddItemTableRow(table, item.get());

    std::string divText = item->File.filename().string();
    if (item->Line >= 0)
    {
        divText += fmt::format(":{}", item->Line);
    }
    main->AddDivision(divText);

    if (!item->Issues.empty())
    {
        main->AddHeading(3, "Issues:");
        for (auto& issue : item->Issues)
        {
            main->AddParagraph(issue)->SetAttribute("style", "color:red;");
        }
    }

    if (isItem)
    {
        main->AddHeading(3, "Rules:");
    }
    else
    {
        main->AddHeading(3, "Items:");
    }

    table = main->AddDivision()->AddTable();
    table->SetAttribute("id", "t01");
    AddItemTableHeader(table);

    for (auto& ref : item->References)
    {
        AddItemTableRow(table, ref);
    }

    return html.ToString();
}

void HtmlGenerator::AddItemTableRow(HtmlElement* table, CsvItem* row)
{
    std::string backgroundColorStyle = "";
    if (row->References.size() == 0)
    {
        backgroundColorStyle = "background-color:#ffc;";
    }
    if (row->Issues.size() > 0)
    {
        backgroundColorStyle = "background-color:#fcc;";
    }

    double value;
    std::string colorStyle{};
    try
    {
        value = std::stod(row->Value);
        if (value < 0)
        {
            colorStyle = "color: #FF0000;";
        }
        else
        {
            colorStyle = "color: #007F00;";
        }
    }
    catch (const std::exception&)
    {
        colorStyle = "";
    }

    auto htmlRow = table->AddTableRow();
    htmlRow->SetAttribute("style", backgroundColorStyle);
    auto cell = htmlRow->AddTableCell();
    AddItemHyperlink(cell, row->Id);

    htmlRow->AddTableCell(row->Category.empty() ? "&nbsp;" : row->Category);
    htmlRow->AddTableCell(row->PayerPayee.empty() ? "&nbsp;" : row->PayerPayee);
    htmlRow->AddTableCell(row->Description.empty() ? "&nbsp;" : row->Description);
    htmlRow->AddTableCell(row->Type.empty() ? "&nbsp;" : row->Type);
    htmlRow->AddTableCell(row->Date.ToString().empty() ? "&nbsp;" : row->Date.ToString());
    htmlRow->AddTableCell(row->Account.empty() ? "&nbsp;" : row->Account);
    cell = htmlRow->AddTableCell(row->Value.empty() ? "&nbsp;" : row->Value);
    cell->SetAttribute("style", colorStyle);
}

} // namespace hokee