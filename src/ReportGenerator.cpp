#include "ReportGenerator.h"
#include "Utils.h"

#include <filesystem>
#include <fmt/core.h>
#include <fmt/format.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>

namespace HomeBanking
{
namespace fs = std::filesystem;

ReportGenerator::ReportGenerator(CsvDatabase* database)
    : _csvDatabase{database}
{
}

std::string ReportGenerator::GetHeadline(const std::string& urlPrefix)
{
    std::stringstream result;
    result << fmt::format("<a href=\"{}{}\">Summary</a> | ", urlPrefix, _indexHtml.string());
    result << fmt::format("<a href=\"{}{}\">All ({})</a> ", urlPrefix, _allHtml.string(),
                          _csvDatabase->Data.size());
    result << fmt::format("<a href=\"{}{}\">Assigned ({})</a> ", urlPrefix, _assignedHtml.string(),
                          _csvDatabase->Assigned.size());

    if (_csvDatabase->Unassigned.size() == 0)
    {
        result << fmt::format("<a href=\"{}{}\">Unassigned ({})</a> | ", urlPrefix, _unassignedHtml.string(),
                              _csvDatabase->Unassigned.size());
    }
    else
    {
        result << fmt::format(
            "<a href=\"{}{}\"><mark style=\"background-color:#ffc;\">Unassigned ({})</mark></a> | ", urlPrefix,
            _unassignedHtml.string(), _csvDatabase->Unassigned.size());
    }

    result << fmt::format("<a href=\"{}{}\">Rules ({})</a> | ", urlPrefix, _rulesHtml.string(),
                          _csvDatabase->Data.size());

    if (_csvDatabase->Issues.size() == 0)
    {
        result << fmt::format("<a href=\"{}{}\">Issues (0)</a> ", urlPrefix, _issuesHtml.string());
    }
    else
    {
        result << fmt::format("<a href=\"{}{}\"><mark style=\"background-color:#fcc;\">Issues ({})</mark></a> ",
                              urlPrefix, _issuesHtml.string(), _csvDatabase->Issues.size());
    }

    return result.str();
}

std::string ReportGenerator::GetIndexPage()
{
    std::vector<std::string> categories;
    categories.push_back("All");
    for (auto& rule : _csvDatabase->Rules)
    {
        if (std::find(categories.begin(), categories.end(), rule->Category) == categories.end())
        {
            categories.push_back(rule->Category);
        }
    }

    std::stringstream htmlPage;

    htmlPage << GetPageStart("Summary");
    int minYear = 3000;
    int maxYear = 1900;

    std::map<int, std::map<int, std::map<std::string, CsvTable>>> database;
    for (auto& item : _csvDatabase->Data)
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

    std::string tableHead = "<tr><th>Date</th>";
    for (auto& cat : categories)
    {
        tableHead += fmt::format("<th>{}</th>", cat);
    }
    tableHead += "</tr>\n";

    for (int year = minYear; year <= maxYear; ++year)
    {
        Utils::PrintInfo(fmt::format("  Write year {}", year));
        htmlPage << tableHead;

        for (int month = 0; month <= 12; ++month)
        {
            std::string rowStyle = "";
            std::string name = fmt::format("{}.{}", year, month);
            if (month == 0)
            {
                name = fmt::format("{}", year);
                rowStyle = " style=\"background-color:#ccf;\"";
            }

            htmlPage << fmt::format("<tr {}><td>{}</td>", rowStyle, name);
            for (auto& cat : categories)
            {
                fs::path fileName = fmt::format("{}_{}_{}.html", cat, year, month);
                std::ofstream htmlPage2;
                htmlPage2.open(_outputDirectory / fileName);
                htmlPage2 << GetTablePage(fmt::format("{}: {}", name, cat), database[year][month][cat]);
                htmlPage2.close();

                double sum = 0;
                for (auto& item : database[year][month][cat])
                {
                    if (item->Category == "Ignorieren" && cat != "Ignorieren")
                    {
                        continue;
                    }
                    sum += std::stod(item->Value);
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

void ReportGenerator::PrintIssues()
{
    Utils::PrintInfo("Found Problems:");
    for (auto& row : _csvDatabase->Issues)
    {
        Utils::PrintInfo("");
        Utils::PrintInfo(fmt::format("- {}:{}:", row->File.string(), row->Line));
        Utils::PrintInfo(fmt::format("    {}", row->ToString()));
        Utils::PrintInfo("  Issues:");
        for (auto& issue : row->Issues)
        {
            Utils::PrintInfo(fmt::format("    {}", issue));
        }
        Utils::PrintInfo("  References:");
        for (auto& ref : row->References)
        {
            Utils::PrintInfo(fmt::format("    {}", ref->ToString()));
        }
    }
}

void ReportGenerator::Write(const fs::path& outputDirectory)
{
    _outputDirectory = outputDirectory;
    if (!fs::exists(_outputDirectory))
    {
        fs::create_directory(_outputDirectory);
    }
    if (!fs::exists(_outputDirectory / _itemsDirectory))
    {
        fs::create_directory(_outputDirectory / _itemsDirectory);
    }

    std::ofstream htmlPage;

    Utils::PrintInfo(fmt::format("  Write {}", _indexHtml.string()));
    htmlPage.open(_outputDirectory / _indexHtml);
    htmlPage << GetIndexPage();
    htmlPage.close();

    Utils::PrintInfo(fmt::format("  Write {}", _allHtml.string()));
    htmlPage.open(_outputDirectory / _allHtml);
    htmlPage << GetTablePage("All", _csvDatabase->Data);
    htmlPage.close();

    Utils::PrintInfo(fmt::format("  Write {}...", _assignedHtml.string()));
    htmlPage.open(_outputDirectory / _assignedHtml);
    htmlPage << GetTablePage("Assigned", _csvDatabase->Assigned);
    htmlPage.close();

    Utils::PrintInfo(fmt::format("  Write {}...", _unassignedHtml.string()));
    htmlPage.open(_outputDirectory / _unassignedHtml);
    htmlPage << GetTablePage("Unassigned", _csvDatabase->Unassigned);
    htmlPage.close();

    Utils::PrintInfo(fmt::format("  Write {}...", _rulesHtml.string()));
    htmlPage.open(_outputDirectory / _rulesHtml);
    htmlPage << GetTablePage("Rules", _csvDatabase->Rules);
    htmlPage.close();

    Utils::PrintInfo(fmt::format("  Write {}...", _issuesHtml.string()));
    htmlPage.open(_outputDirectory / _issuesHtml);
    htmlPage << GetTablePage("Issues", _csvDatabase->Issues);
    htmlPage.close();

    Utils::PrintInfo("  Write items...");
    int itemCount = 0;
    for (auto& row : _csvDatabase->Data)
    {
        if (itemCount++ % 100 == 0)
        {
            Utils::PrintInfo(fmt::format("  Write item {}/{}", itemCount, _csvDatabase->Data.size()));
        }
        htmlPage.open(_outputDirectory / _itemsDirectory / fmt::format("{:#04}.html", row->Id));
        htmlPage << GetItemPage(fmt::format("Item {}:", row->Id), row);
        htmlPage.close();
    }

    Utils::PrintInfo("  Write rules...");
    int ruleCount = 0;
    for (auto& rule : _csvDatabase->Rules)
    {
        if (ruleCount++ % 100 == 0)
        {
            Utils::PrintInfo(fmt::format("  Write rule {}/{}", ruleCount, _csvDatabase->Rules.size()));
        }
        htmlPage.open(_outputDirectory / _itemsDirectory / fmt::format("{:#04}.html", rule->Id));
        htmlPage << GetItemPage(fmt::format("Rule {}:", rule->Id), rule);
        htmlPage.close();
    }
}

std::string ReportGenerator::GetTablePage(const std::string& title, const CsvTable& data)
{
    std::stringstream htmlPage;
    htmlPage << GetPageStart(title);
    htmlPage << GetTableStart();
    for (const auto& row : data)
    {
        htmlPage << GetTableRow(row.get());
    }
    htmlPage << GetTableEnd();
    htmlPage << GetPageEnd();
    return htmlPage.str();
}

std::string ReportGenerator::GetItemPage(const std::string& title, const std::shared_ptr<CsvItem>& row)
{
    std::stringstream htmlPage;
    htmlPage << GetPageStart(title, "../");
    htmlPage << GetTableStart();
    htmlPage << GetTableRow(row.get(), "../");
    htmlPage << GetTableEnd();

    htmlPage << fmt::format("<p>Source: <a href=\"../../{}\">{} : {}</a></p>\n", row->File.string(),
                            row->File.filename().string(), row->Line);

    htmlPage << "<h3>Issues: </h3>\n";
    for (auto& issue : row->Issues)
    {
        htmlPage << "<p style=\"color:red;\">" << issue << "</p>";
    }

    htmlPage << "<h3>References: </h3>\n";
    htmlPage << GetTableStart();
    for (auto& ref : row->References)
    {
        htmlPage << GetTableRow(ref, "../");
    }
    htmlPage << GetTableEnd();
    htmlPage << GetPageEnd();
    return htmlPage.str();
}

std::string ReportGenerator::GetPageStart(const std::string& title, const std::string& urlPrefix)
{
    const std::string head = "<!DOCTYPE html>\n"
                             "<html>\n"
                             "<head>\n"
                             "<style>\n"
                             "table {\n"
                             "  width:100%;\n"
                             "}\n"
                             "table, th, td {\n"
                             "  border: 1px solid #008;\n"
                             "  border-collapse: collapse;\n"
                             "}\n"
                             "th, td {\n"
                             "  padding: 5px;\n"
                             "  text-align: left;\n"
                             "}\n"
                             "table#t01 tr:nth-child(even) {\n"
                             "  background-color: #eef;\n"
                             "}\n"
                             "table#t01 tr:nth-child(odd) {\n"
                             " background-color: #fff;\n"
                             "}\n"
                             "table#t01 th {\n"
                             "  background-color: #008;\n"
                             "  color: white;\n"
                             "}\n"
                             "</style>\n"
                             "</head>\n";

    const std::string htmlBodyStart = fmt::format("<body>\n{}\n", GetHeadline(urlPrefix));
    const std::string htmlTitle = fmt::format("<h2>{}</h2>\n", title);

    return head + htmlBodyStart + htmlTitle;
}

std::string ReportGenerator::GetPageEnd()
{
    return "</body>\n"
           "</html>";
}

std::string ReportGenerator::GetTableStart()
{
    return "<table id=\"t01\">\n"
           "<tr><th>#</th><th>Category</th><th>Payer/Payee</th><th>Description</th>"
           "<th>Type</th><th>Date</th><th>Account</th><th>Value</th></tr>\n";
}

std::string ReportGenerator::GetTableRow(CsvItem* row, const std::string& urlPrefix)
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

    const std::string rowFormat = "<tr{9}><td><a "
                                  "href=\"{8}/{10}{0:#04}.html\">{0:#04}</a></td><td>{1}</"
                                  "td><td>{2}</td><td>{3}</td><td>{4}</"
                                  "td><td>{5}</td><td>{6}</td><td>{7}</td></tr>\n";
    std::string htmlTableRow
        = fmt::format(rowFormat, row->Id, row->Category, row->PayerPayee, row->Description, row->Type, row->Date.ToString(),
                      row->Account, row->Value, _itemsDirectory.string(), style, urlPrefix);
    return htmlTableRow;
}

std::string ReportGenerator::GetTableEnd()
{
    return "</table>\n";
}

} // namespace HomeBanking