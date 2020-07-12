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
#include <sstream>

namespace hokee
{

std::string HtmlGenerator::GetHeader(CsvDatabase* pDatabase)
{
    std::stringstream result;

    result << "<header style=\"border-bottom: 1px solid black;\">";
    result << "<table cellspacing=\"0\" cellpadding=\"0\" style=\"border: hidden\"><tr>";

    const std::string fmtButton
        = "<td style=\"border: hidden\"><a href=\"{}\"><main style=\"text-align:center;\"><img "
          "src=\"{}\"></main><footer style=\"text-align:center;\">{}</footer></a></td>";
    result << fmt::format(fmtButton, INDEX_HTML, "money.png", "Summary");

    result << fmt::format(fmtButton, ALL_HTML, "database.png",
                          fmt::format("All({})", pDatabase->Data.size()));

    result << fmt::format(fmtButton, ASSIGNED_HTML, "sign-check.png",
                          fmt::format("Assigned ({})", pDatabase->Assigned.size()));

    if (pDatabase->Unassigned.size() == 0)
    {
        result << fmt::format(fmtButton, UNASSIGNED_HTML, "sign-delete2.png",
                              fmt::format("Unassigned ({})", pDatabase->Unassigned.size()));
    }
    else
    {
        result << fmt::format(fmtButton, UNASSIGNED_HTML, "sign-delete.png",
                              fmt::format("Unassigned ({})", pDatabase->Unassigned.size()));
    }

    if (pDatabase->Issues.size() == 0)
    {
        result << fmt::format(fmtButton, ISSUES_HTML, "sign-warning2.png",
                              fmt::format("Issues ({})", pDatabase->Issues.size()));
    }
    else
    {
        result << fmt::format(fmtButton, ISSUES_HTML, "sign-warning.png",
                              fmt::format("Issues ({})", pDatabase->Issues.size()));
    }

    result << fmt::format(fmtButton, RULES_HTML, "notepad.png",
                          fmt::format("Rules ({})", pDatabase->Rules.size()));

    result << "<td style=\"border: hidden\" width=\"99%\"></td>";

    result << fmt::format(fmtButton, UPDATE_HTML, "sign-sync.png", "Update");
    result << fmt::format(fmtButton, SETTINGS_HTML, "cogs.png", "Settings");
    result << fmt::format(fmtButton, EXIT_HTML, "sign-error.png", "Exit");

    result << "</tr></table>";

    result << "</header>";
    return result.str();
}

std::string HtmlGenerator::GetSummaryPage(CsvDatabase* pDatabase)
{
    std::vector<std::string> categories{""};
    for (auto& rule : pDatabase->Rules)
    {
        if (std::find(categories.begin(), categories.end(), rule->Category) == categories.end())
        {
            categories.push_back(rule->Category);
        }
    }

    std::stringstream htmlPage;

    htmlPage << GetItemStart(pDatabase, "Summary");
    int minYear = 3000;
    int maxYear = 1900;

    std::map<int, std::map<int, std::map<std::string, CsvTable>>> database;
    for (auto& item : pDatabase->Data)
    {
        const int year = item->Date.GetYear();
        const int month = item->Date.GetMonth();
        database[year][0][categories[0]].push_back(item);
        database[year][0][item->Category].push_back(item);
        database[year][month][categories[0]].push_back(item);
        database[year][month][item->Category].push_back(item);

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
        tableHead += fmt::format("<th>{}</th>", categories[i]);
    }
    tableHead += "</tr>\n";

    for (int year = minYear; year <= maxYear; ++year)
    {
        htmlPage << tableHead;

        for (int month = 0; month <= 12; ++month)
        {
            std::string rowStyle = "";
            std::string name = fmt::format("{}-{}", month, year);
            if (month == 0)
            {
                name = fmt::format("{}", year);
                rowStyle = " style=\"background-color:#ccf;\"";
            }

            htmlPage << fmt::format("<tr {}><td>{}</td>", rowStyle, name);
            for (auto& cat : categories)
            {
                const fs::path fileName = fmt::format("{}?year={}&month={}&category={}", ITEMS_HTML, year, month, cat);

                double sum = 0;
                for (auto& item : database[year][month][cat])
                {
                    if (item->Category == "Ignorieren" && cat != "Ignorieren")
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
                const std::string rowFormat = "<td {}><a href=\"{}\">{:.2f}&euro;</a></td>\n";
                htmlPage << fmt::format(rowFormat, cellStyle, fileName.string(), sum);
            }
            htmlPage << "</tr>";
        }
    }
    htmlPage << "</table>\n";

    htmlPage << "</body>\n"
                "</html>";
    return htmlPage.str();
}

std::string HtmlGenerator::GetTablePage(CsvDatabase* pDatabase, const std::string& title, const CsvTable& data)
{
    std::stringstream htmlPage;
    htmlPage << GetItemStart(pDatabase, title);
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
    return fmt::format("<!DOCTYPE html>\n"
                       "<html>\n"
                       "  <body style=\"padding: 50px 0;\">\n"
                       "    <div style=\"text-align:center;\n"
                       "                 color: red;\n"
                       "                 margin: auto;\n"
                       "                 width: 50%;\n"
                       "                 padding: 20px;\n"
                       "                 background: #F0F0F0;\n"
                       "                 border: 1px solid #DDD;\n"
                       "                 box-shadow: 3px 3px 0px rgba(0,0,0, .2);\">\n"
                       "    <p><img src=\"sign-ban.png\"/></p>\n"
                       "    <p><b>ERROR {} - {} </b></p>\n"
                       "  </div>\n"
                       "</body>\n"
                       "</html>",
                       errorCode, errorMessage);
}

std::string HtmlGenerator::GetProgressPage(int value, int max, const std::string& m1, const std::string& m2,
                                           const std::string& m3, const std::string& m4)
{
    return fmt::format("<!DOCTYPE html>\n"
                       "<html>\n"
                       "  <head>\n"
                       "    <meta http-equiv=\"refresh\" content=\"0.33\">\n"
                       "  </head>\n"
                       "  <body style=\"padding: 50px 0;\">\n"
                       "    <div style=\"text-align:center;\n"
                       "                 margin: auto;\n"
                       "                 width: 50%;\n"
                       "                 padding: 20px;\n"
                       "                 background: #F0F0F0;\n"
                       "                 border: 1px solid #DDD;\n"
                       "                 box-shadow: 3px 3px 0px rgba(0,0,0, .2);\">\n"
                       "    <div style=\"color: #D0D0D0\">{}</div>\n"
                       "    <div style=\"color: #B0B0B0\">{}</div>\n"
                       "    <div style=\"color: #707070\">{}</div>\n"
                       "    <div style=\"color: #000000\">{}</div>\n"
                       "    <div><progress value=\"{}\" max=\"{}\"/></div>\n"
                       "  </div>\n"
                       "</body>\n"
                       "</html>",
                       m1, m2, m3, m4, value, max);
}

std::string HtmlGenerator::GetItemPage(CsvDatabase* pDatabase, int id)
{
    std::string title = "";
    bool isItem = false;
    std::shared_ptr<CsvItem> item = nullptr;
    for (auto& i : pDatabase->Data)
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
        for (auto& r : pDatabase->Rules)
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
    htmlPage << GetItemStart(pDatabase, title);
    htmlPage << GetTableStart();
    htmlPage << GetTableRow(item.get());
    htmlPage << GetTableEnd();

    htmlPage << fmt::format("<p>Source: <a href=\"../../{}\">{} : {}</a></p>\n", item->File.string(),
                            item->File.filename().string(), item->Line);

    htmlPage << "<h3>Issues: </h3>\n";
    for (auto& issue : item->Issues)
    {
        htmlPage << "<p style=\"color:red;\">" << issue << "</p>";
    }

    if(isItem)
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

std::string HtmlGenerator::GetItemStart(CsvDatabase* pDatabase, const std::string& title)
{
    const std::string htmlHead
        = "<!DOCTYPE html>\n"
          "<html>\n"
          "  <head>\n"
          "  <title>hokee</title>\n"
          "  <meta name=\"application-name\" content=\"hokee\">\n"
          "  <meta charset=\"UTF-8\">\n"
          "  <link rel=\"apple-touch-icon\" sizes=\"180x180\" href=\"/apple-touch-icon.png\">\n"
          "  <link rel=\"icon\" type=\"image/png\" sizes=\"32x32\" href=\"/favicon-32x32.png\">\n"
          "  <link rel=\"icon\" type=\"image/png\" sizes=\"16x16\" href=\"/favicon-16x16.png\">\n"
          "  <link rel=\"mask-icon\" href=\"/safari-pinned-tab.svg\" color=\"#5bbad5\">\n"
          "  <style>\n"
          "    #left {width: 10%; display: inline-block; text-align:left;}\n"
          "    #middle {width: 80%; display: inline-block; text-align:center;}\n"
          "    #right {width: 10%; display: inline-block; text-align:right;}\n"
          "    table {\n"
          "      width:100%;\n"
          "    }\n"
          "    table, th, td {\n"
          "      border: 1px solid #008;\n"
          "      border-collapse: collapse;\n"
          "    }\n"
          "    th, td {\n"
          "      padding: 5px;\n"
          "      text-align: left;\n"
          "    }\n"
          "    table#t01 tr:nth-child(even) {\n"
          "      background-color: #eef;\n"
          "    }\n"
          "    table#t01 tr:nth-child(odd) {\n"
          "      background-color: #fff;\n"
          "    }\n"
          "    table#t01 th {\n"
          "      background-color: #008;\n"
          "      color: white;\n"
          "    }\n"
          "  </style>\n"
          "</head>\n";

    const std::string htmlBodyStart = fmt::format("<body>\n{}\n", GetHeader(pDatabase));
    const std::string htmlTitle = fmt::format("<h2>{}</h2>\n", title);

    return htmlHead + htmlBodyStart + htmlTitle;
}

std::string HtmlGenerator::GetItemEnd()
{
    return "</body>\n"
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
    std::string style = "";
    if (row->References.size() == 0)
    {
        style = " style=\"background-color:#ffc;\"";
    }
    if (row->Issues.size() > 0)
    {
        style = " style=\"background-color:#fcc;\"";
    }

    const std::string rowFormat = "<tr{8}><td><a "
                                  "href=\"{9}?id={0:#04}\">{0:#04}</a></td><td>{1}</"
                                  "td><td>{2}</td><td>{3}</td><td>{4}</"
                                  "td><td>{5}</td><td>{6}</td><td>{7}</td></tr>\n";
    std::string htmlTableRow = fmt::format(rowFormat, row->Id, row->Category, row->PayerPayee, row->Description,
                                           row->Type, row->Date.ToString(), row->Account, row->Value, style, ITEM_HTML);
    return htmlTableRow;
}

std::string HtmlGenerator::GetTableEnd()
{
    return "</table>\n";
}

} // namespace hokee