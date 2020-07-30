#include "HtmlGenerator.h"
#include "InternalException.h"
#include "Utils.h"

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
std::string HtmlGenerator::GetButton(const std::string& link, const std::string& tooltip, const std::string& image,
                                     const std::string& text)
{
    return fmt::format(
        "<td style=\"border: hidden\"><a href=\"{}\" title=\"{}\"><main style=\"text-align:center;\"><img "
        "src=\"{}\"/></main><footer style=\"text-align:center;\">{}</footer></a></td>", link, tooltip, image, text);
}

std::string HtmlGenerator::GetHeader(const CsvDatabase& database)
{
    std::stringstream result;

    result << "    <header style=\"border-bottom: 1px solid black;\">";
    result << "<table cellspacing=\"0\" cellpadding=\"0\" style=\"border: hidden\"><tr>";

    result << GetButton(INDEX_HTML, "Show Summary", "48-money.png", "Summary");

    result << GetButton(RULES_HTML, "Show Rules", "48-file-excel.png",
                          fmt::format("Rules ({})", database.Rules.size()));

    result << GetButton(ALL_HTML, "Show All Items", "48-file-text.png",
                          fmt::format("All&nbsp;Items ({})", database.Data.size()));

    result << GetButton(ASSIGNED_HTML, "Show Assigned Items", "48-sign-check.png",
                          fmt::format("Assigned ({})", database.Assigned.size()));

    if (database.Unassigned.size() == 0)
    {
        result << GetButton(UNASSIGNED_HTML, "Show Unassigned Items", "48-sign-delete2.png",
                              fmt::format("Unassigned ({})", database.Unassigned.size()));
    }
    else
    {
        result << GetButton(UNASSIGNED_HTML, "Show Unassigned Items", "48-sign-delete.png",
                              fmt::format("Unassigned ({})", database.Unassigned.size()));
    }

    if (database.Issues.size() == 0)
    {
        result << GetButton(ISSUES_HTML, "Show Issues", "48-sign-warning2.png",
                              fmt::format("Issues ({})", database.Issues.size()));
    }
    else
    {
        result << GetButton(ISSUES_HTML, "Show Issues", "48-sign-warning.png",
                              fmt::format("Issues ({})", database.Issues.size()));
    }

    result << "<td style=\"border: hidden\" width=\"99%\"></td>";

    result << GetButton(RELOAD_CMD, "Reload CSV Data", "48-sign-sync.png", "Reload");
    result << GetButton(INPUT_CMD, "Open Input Folder", "48-box-full.png", "Input");
    result << GetButton(SEARCH_HTML, "Open Search Page", "48-search.png", "Search");
    result << GetButton(SETTINGS_CMD, "Open Settings File", "48-cogs.png", "Settings");
    result << GetButton(EXIT_CMD, "Stop hokee", "48-sign-error.png", "Exit");

    result << "</tr></table>";

    result << "</header>";
    return result.str();
}

std::string HtmlGenerator::GetSummaryPage(const CsvDatabase& database)
{
    std::stringstream htmlPage;
    htmlPage << GetItemStart(database, "Summary");

    std::vector<std::string> categories{""};
    for (auto& rule : database.Rules)
    {
        if (std::find(categories.begin(), categories.end(), rule->Category) == categories.end())
        {
            categories.push_back(rule->Category);
        }
    }

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

    htmlPage << "<table id=\"t01\">\n";

    std::string tableHead = "<tr><th>Date</th><th>*</th>";
    for (size_t i = 1; i < categories.size(); ++i)
    {
        std::string cat = categories[i];
        if (!cat.empty() && cat.back() == '!')
        {
            // remove tailing "!"
            cat.pop_back();
        }
        tableHead += fmt::format("<th>{}</th>", cat);
    }
    tableHead += "</tr>\n";

    for (int year = minYear; year <= maxYear; ++year)
    {
        htmlPage << tableHead;

        for (int month = 0; month <= 12; ++month)
        {
            std::string rowStyle = "";
            std::string name = fmt::format("{}.{}", month, year);
            if (month == 0)
            {
                name = fmt::format("{}", year);
                rowStyle = " style=\"background-color:#ccf;\"";
            }

            htmlPage << fmt::format("<tr {}><td>{}</td>", rowStyle, name);
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
                    if (month == 0)
                    {
                        cellStyle = " style=\"background-color:#ada;\"";
                    }
                    else
                    {
                        cellStyle = " style=\"background-color:#cfc;\"";
                    }
                }
                if (sum < 0)
                {
                    if (month == 0)
                    {
                        cellStyle = " style=\"background-color:#daa;\"";
                    }
                    else
                    {
                        cellStyle = " style=\"background-color:#fcc;\"";
                    }
                }
                htmlPage << fmt::format("<td {}>", cellStyle);
                GetItemsReference(htmlPage, year, month, cat, fmt::format("{:.2f}&euro;", sum));
                htmlPage << "</td>\n";
            }
            htmlPage << "</tr>";
        }
    }
    htmlPage << "</table>\n";

    htmlPage << "</body>\n"
                "</html>";
    return htmlPage.str();
}

std::string HtmlGenerator::GetTablePage(const CsvDatabase& database, const std::string& title,
                                        const CsvTable& data)
{
    std::stringstream htmlPage;
    htmlPage << GetItemStart(database, title);
    htmlPage << GetTableStart();
    for (const auto& row : data)
    {
        htmlPage << GetTableRow(row.get());
    }
    htmlPage << GetTableEnd();
    htmlPage << GetItemEnd();
    return htmlPage.str();
}

std::string HtmlGenerator::GetErrorPage(int errorCode, const std::string& errorMessage)
{
    Utils::PrintError(fmt::format("HttpServer operation failed: {}", errorMessage));
    std::stringstream htmlPage{};
    htmlPage << "<!DOCTYPE html>\n"
                "<html>\n"
                "  <head>\n";
    htmlPage << GetHead();
    htmlPage << "  </head>\n"
                "  <body>\n";

    htmlPage << fmt::format(
        "    <main style=\"padding: 50px 0;\">\n"
        "      <div style=\"text-align:center;\n"
        "                   color: red;\n"
        "                   margin: auto;\n"
        "                   width: 80%;\n"
        "                   padding: 20px;\n"
        "                   background: #F0F0F0;\n"
        "                   border: 1px solid #DDD;\n"
        "                   box-shadow: 3px 3px 0px rgba(0,0,0, .2);\">\n"
        "        <p><a href=\"{}\" title=\"Stop hokee\"><img src=\"96-sign-ban.png\"/></a></p>\n"
        "        <h2>ERROR {}</h2>\n"
        "        <p><b>{}</b></p>\n"
        "      </div>\n"
        "    </main>\n",
        EXIT_CMD, errorCode, errorMessage);
    htmlPage << "  </body>\n"
                "</html>";
    return htmlPage.str();
}

std::string HtmlGenerator::GetEmptyInputPage()
{
    std::stringstream htmlPage{};
    htmlPage << "<!DOCTYPE html>\n"
                "<html>\n"
                "  <head>\n";
    htmlPage << GetHead();
    htmlPage << "  </head>\n"
                "  <body>\n";

    htmlPage << "    <main style=\"padding: 50px 0;\">\n"
                "      <div style=\"text-align:center;\n"
                "                   margin: auto;\n"
                "                   width: 80%;\n"
                "                   padding: 20px;\n"
                "                   background: #F0F0F0;\n"
                "                   border: 1px solid #DDD;\n"
                "                   box-shadow: 3px 3px 0px rgba(0,0,0, .2);\">\n";
    htmlPage << fmt::format(
        "        <p><a href=\"{}\" title=\"Open Input Folder\"><img src=\"96-box.png\"/></a></p>\n", INPUT_CMD);
    htmlPage << "        <h2>Could not find any input data!</h2>\n"
                "        <p>\n"
                "          <table cellspacing=\"0\" cellpadding=\"0\" style=\"border: hidden\"><tr>";

    htmlPage << "<td style=\"border: hidden\" width=\"50%\"></td>";
    htmlPage << GetButton(COPY_SAMPLES_CMD, "Copy Samples", "48-box-in.png", "Copy&nbsp;Samples");
    htmlPage << GetButton(RELOAD_CMD, "Reload", "48-sign-sync.png", "Reload");
    htmlPage << GetButton(SETTINGS_CMD, "Open Settings", "48-cogs.png", "Open&nbsp;Settings");
    htmlPage << GetButton(EXIT_CMD, "Stop hokee", "48-sign-error.png", "Exit");
    htmlPage << "<td style=\"border: hidden\" width=\"50%\"></td>";

    htmlPage << "        </tr></table>\n"
                "        </p>\n"
                "      </div>\n"
                "    </main>\n"
                "  </body>\n"
                "</html>";

    return htmlPage.str();
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

    std::stringstream htmlPage{};
    htmlPage << "<!DOCTYPE html>\n"
                "<html>\n"
                "  <head>\n";
    htmlPage << GetHead();
    htmlPage << "    <meta http-equiv=\"refresh\" content=\"1\">\n"
                "  </head>\n"
                "  <body>\n";
    htmlPage << fmt::format("    <main style=\"padding: 50px 0;\">\n"
                            "      <div style=\"text-align:center;\n"
                            "                   margin: auto;\n"
                            "                   width: 80%;\n"
                            "                   padding: 20px;\n"
                            "                   background: #F0F0F0;\n"
                            "                   border: 1px solid #DDD;\n"
                            "                   box-shadow: 3px 3px 0px rgba(0,0,0, .2);\">\n"
                            "        <p>&nbsp;</p>\n"
                            "        <div style=\"color: #E0E0E0\">{}</div>\n"
                            "        <div style=\"color: #C0C0C0\">{}</div>\n"
                            "        <div style=\"color: #A0A0A0\">{}</div>\n"
                            "        <div style=\"color: #808080\">{}</div>\n"
                            "        <div style=\"color: #606060\">{}</div>\n"
                            "        <div style=\"color: #$04040\">{}</div>\n"
                            "        <div style=\"color: #202020\">{}</div>\n"
                            "        <div style=\"color: #000000\">{}</div>\n"
                            "        <p><progress value=\"{}\" max=\"{}\"/></p>\n"
                            "      </div>\n"
                            "    </main>\n",
                            m7, m6, m5, m4, m3, m2, m1, m0, value, max);
    htmlPage << "  </body>\n"
                "</html>";
    return htmlPage.str();
}

void HtmlGenerator::GetItemReference(std::stringstream& output, int id)
{
    output << fmt::format("<a href=\"{0}?id={1}\">{1:#04}</a>", HtmlGenerator::ITEM_HTML, id);
}

void HtmlGenerator::GetItemsReference(std::stringstream& output, int year, int month, const std::string& category,
                                      const std::string& text)
{
    output << fmt::format("<a href=\"{}?year={}&month={}&category={}\">{}</a>", ITEMS_HTML, year, month, category,
                          text);
}

std::string HtmlGenerator::GetEditorReference(const fs::path& file)
{
    std::stringstream output;
    output << fmt::format("<a href=\"{}?file={}\"><img src=\"24-notepad.png\"/></a>\n", HtmlGenerator::EDIT_CMD,
                          file.string());
    output << fmt::format("<a href=\"{}?folder={}\"><img src=\"24-folder.png\"/></a>", HtmlGenerator::OPEN_CMD,
                          file.parent_path().string());

    fs::path formatFile = file.parent_path() / "format.ini";
    if (fs::exists(formatFile))
    {
        output << fmt::format("<a href=\"{}?file={}\"><img src=\"24-wrench-screwdriver.png\"/></a>",
                              HtmlGenerator::EDIT_CMD, formatFile.string());
    }
    return output.str();
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

    std::stringstream htmlPage;

    title += GetEditorReference(item->File);
    htmlPage << GetItemStart(database, title);
    htmlPage << "<div>";
    htmlPage << item->File.filename().string();
    if (item->Line >= 0)
    {
        htmlPage << fmt::format(":{}", item->Line);
    }
    htmlPage << "</div>\n";
    
    htmlPage << "<p>\n";

    htmlPage << GetTableStart();
    htmlPage << GetTableRow(item.get());
    htmlPage << GetTableEnd();

    htmlPage << "</p>\n";

    htmlPage << "<h3>Issues: </h3>\n";
    for (auto& issue : item->Issues)
    {
        htmlPage << "<p style=\"color:red;\">" << issue << "</p>";
    }

    if (isItem)
    {
        htmlPage << "<h3>Rules: </h3>\n";
    }
    else
    {
        htmlPage << "<h3>Items: </h3>\n";
    }
    htmlPage << GetTableStart();
    for (auto& ref : item->References)
    {
        htmlPage << GetTableRow(ref);
    }
    htmlPage << GetTableEnd();
    htmlPage << GetItemEnd();
    return htmlPage.str();
}

std::string HtmlGenerator::GetHead()
{
    return "    <title>hokee</title>\n"
           "    <meta name=\"application-name\" content=\"hokee\">\n"
           "    <meta charset=\"UTF-8\">\n"
           "    <link rel=\"apple-touch-icon\" sizes=\"180x180\" href=\"/apple-touch-icon.png\">\n"
           "    <link rel=\"icon\" type=\"image/png\" sizes=\"32x32\" href=\"/favicon-32x32.png\">\n"
           "    <link rel=\"icon\" type=\"image/png\" sizes=\"16x16\" href=\"/favicon-16x16.png\">\n"
           "    <link rel=\"mask-icon\" href=\"/safari-pinned-tab.svg\" color=\"#5bbad5\">\n"
           "    <style>\n"
           "      #left {width: 10%; display: inline-block; text-align:left;}\n"
           "      #middle {width: 80%; display: inline-block; text-align:center;}\n"
           "      #right {width: 10%; display: inline-block; text-align:right;}\n"
           "      table {\n"
           "        width:100%;\n"
           "      }\n"
           "      table, th, td {\n"
           "        border: 1px solid #008;\n"
           "        border-collapse: collapse;\n"
           "      }\n"
           "      th, td {\n"
           "        padding: 5px;\n"
           "        text-align: left;\n"
           "      }\n"
           "      table#t01 tr:nth-child(even) {\n"
           "        background-color: #eef;\n"
           "      }\n"
           "      table#t01 tr:nth-child(odd) {\n"
           "        background-color: #fff;\n"
           "      }\n"
           "      table#t01 th {\n"
           "        background-color: #008;\n"
           "        color: white;\n"
           "      }\n"
           "    </style>\n";
}

std::string HtmlGenerator::GetItemStart(const CsvDatabase& database, const std::string& title)
{
    std::stringstream htmlPage{};
    htmlPage << "<!DOCTYPE html>\n"
                "<html>\n"
                "  <head>\n";
    htmlPage << GetHead();
    htmlPage << "  </head>\n"
                "  <body>\n";
    htmlPage << GetHeader(database);
    htmlPage << "    <main>\n";
    htmlPage << fmt::format("      <h2>{}</h2>\n", title);

    return htmlPage.str();
}

std::string HtmlGenerator::GetItemEnd()
{
    return "    </main>\n"
           "  </body>\n"
           "</html>";
}

std::string HtmlGenerator::GetTableStart()
{
    return "<table id=\"t01\">\n"
           "<tr><th>#</th><th>Category</th><th>Payer/Payee</th><th>Description</th>"
           "<th>Type</th><th>Date</th><th>Account</th><th>Value</th></tr>\n";
}

std::string HtmlGenerator::GetTableRow(CsvItem* row)
{
    std::string backgroundColor = "";
    if (row->References.size() == 0)
    {
        backgroundColor = " style=\"background-color:#ffc;\"";
    }
    if (row->Issues.size() > 0)
    {
        backgroundColor = " style=\"background-color:#fcc;\"";
    }

    double value;
    std::string color{};
    try
    {
        value = std::stod(row->Value);
        if (value < 0)
        {
            color = " style=\"color: #FF0000;\"";
        }
        else
        {
            color = " style=\"color: #007F00;\"";
        }
    }
    catch (const std::exception&)
    {
        color = "";
    }

    std::stringstream htmlTableRow{};
    htmlTableRow << "<tr" << backgroundColor << "><td>";
    GetItemReference(htmlTableRow, row->Id);
    const std::string rowFormat = "</td><td>{}</"
                                  "td><td>{}</td><td>{}</td><td>{}</"
                                  "td><td>{}</td><td>{}</td><td{}>{}</td></tr>\n";
    htmlTableRow << fmt::format(rowFormat, row->Category, row->PayerPayee, row->Description, row->Type,
                                row->Date.ToString(), row->Account, color, row->Value);
    return htmlTableRow.str();
}

std::string HtmlGenerator::GetTableEnd()
{
    return "</table>\n";
}

} // namespace hokee