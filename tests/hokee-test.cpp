#include "Application.h"
#include "InternalException.h"
#include "Utils.h"
#include "hokee.h"
#include "html/HtmlElement.h"

#include <fmt/format.h>

#include <exception>
#include <iostream>
#include <map>

using namespace hokee;

bool runTest(const std::string& name, std::function<bool(void)> test)
{
    Utils::PrintInfo(name);
    Utils::PrintInfo("---------------------------------------------------------------------");
    bool success = test();

    if (success)
    {
        Utils::PrintInfo(" => TEST PASSED");
    }
    else
    {
        Utils::PrintError(" => TEST FAILED");
    }
    Utils::PrintInfo("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    return success;
}

bool FormatTest()
{
    bool success = true;
    Settings config;
    std::string configPath = "../test_data/settings.ini";
    config.SetRuleSetFile("rules.csv");
    config.SetInputDirectory("input2");
    config.Save(configPath);
    const char* testArgv[] = {"hokee", configPath.c_str(), nullptr};
    int testArgc = sizeof(testArgv) / sizeof(testArgv[0]) - 1;
    auto app = std::make_unique<Application>(testArgc, testArgv);
    std::unique_ptr<CsvDatabase> database = app->RunBatch();

    // Compute Hash
    std::map<std::string, int64_t> hashes;
    int64_t sum = 0;
    for (auto& rule : database->Data)
    {
        sum += static_cast<int64_t>(std::stod(rule->Value));
        hashes[rule->Category] += static_cast<int64_t>(std::stod(rule->Value));
        hashes[rule->Category] *= rule->Date.GetYear();
        hashes[rule->Category] /= rule->Date.GetMonth();
        hashes[rule->Category] -= rule->Date.GetDay();
    }
    int64_t hash = 1;
    for (auto& h : hashes)
    {
        hash *= h.second;
    }

    // Check results
    const int64_t expectedHash = -198114973815180800;
    if (hash != expectedHash)
    {
        Utils::PrintError(fmt::format("Computed hash {} does not match expeted hash {} !", hash, expectedHash));
        success = false;
    }
    const int64_t expectedSum = -19886;
    if (sum != expectedSum)
    {
        Utils::PrintError(fmt::format("Computed sum {} does not match expeted sum {} !", sum, expectedSum));
        success = false;
    }
    const int64_t expectedAssignedSize = 56;
    if (database->Assigned.size() != expectedAssignedSize)
    {
        Utils::PrintError(fmt::format("Number of assigned items {} does not match expeted count {} !",
                                      database->Assigned.size(), expectedAssignedSize));
        success = false;
    }
    const int64_t expectedUnassignedSize = 5;
    if (database->Unassigned.size() != expectedUnassignedSize)
    {
        Utils::PrintError(fmt::format("Number of unassigned items {} does not match expeted count {} !",
                                      database->Unassigned.size(), expectedUnassignedSize));
        success = false;
    }
    const int64_t expectedIssuesSize = 5;
    if (database->Issues.size() != expectedIssuesSize)
    {
        Utils::PrintError(fmt::format("Number of issues {} does not match expeted count {} !",
                                      database->Issues.size(), expectedIssuesSize));
        success = false;
    }
    const int64_t expectedRulesSize = 8;
    if (database->Rules.size() != expectedRulesSize)
    {
        Utils::PrintError(fmt::format("Number of found rules {} does not match expeted count {} !",
                                      database->Rules.size(), expectedRulesSize));
        success = false;
    }
    
    return success;
}

bool HtmlTest()
{
    HtmlElement html;
    auto head = html.AddHead();
    head->AddTitle("Title");

    auto meta = head->AddMeta();
    meta->SetAttribute("name", "html_test");
    meta->SetAttribute("content", "unit-test");
    meta->SetAttribute("charset", "UTF-8");

    auto link = head->AddLink();
    link->SetAttribute("rel", "icon");
    link->SetAttribute("type", "image/jpg");
    link->SetAttribute("sizes", "32x32");
    link->SetAttribute("href", "https://www.fillmurray.com/32/32");

    auto body = html.AddBody();
    auto header = body->AddHeader();
    header->AddHeading(2, "Heading 2");

    auto main = body->AddMain();
    auto div = main->AddDivision();
    auto table = div->AddTable();
    auto row = table->AddTableRow();
    row->AddTableHeaderCell("Head 1");
    row->AddTableHeaderCell("Head 2");
    row = table->AddTableRow();
    auto cell = row->AddTableCell();
    cell->AddText("AAA BBB");
    cell->AddBreak();
    cell->AddText("CCC &lt;escaped&gt;");
    cell->AddBreak();
    cell->AddText("DDD &quot;EEE&quot;");
    cell->AddBreak();
    cell->AddText("GGG.");
    cell = row->AddTableCell();
    cell->AddImage("https://www.fillmurray.com/200/250", "fillmurray", 200, 250);

    auto footer = body->AddFooter();
    auto p = footer->AddParagraph();
    p->AddBold("BOLD: ");
    p->AddHyperlink("https://www.fillmurray.com", "Link to fillmurray.com", "www.fillmurray.com");

    fs::path htmlPath = Utils::GetTempDir() / "index.html";
    std::ofstream ofStream;
    ofStream.open(htmlPath, std::ios::binary);
    ofStream << html;
    ofStream.close();

    fs::path referencePath = fs::path("..") / "test_data" / "html_test" / "index.html";

    return Utils::CompareFiles(htmlPath, referencePath);
}

bool RuleTest()
{
    bool success = true;
    Settings config;
    std::string configPath = "../test_data/settings.ini";
    config.SetRuleSetFile("rules.csv");
    config.SetInputDirectory("input1");
    config.Save(configPath);
    const char* testArgv[] = {"hokee", configPath.c_str(), nullptr};
    int testArgc = sizeof(testArgv) / sizeof(testArgv[0]) - 1;
    auto app = std::make_unique<Application>(testArgc, testArgv);
    std::unique_ptr<CsvDatabase> database = app->RunBatch();

    // Compute Hash
    std::map<std::string, int64_t> hashes;
    int64_t sum = 0;
    for (auto& rule : database->Data)
    {
        sum += static_cast<int64_t>(std::stod(rule->Value));
        hashes[rule->Category] += static_cast<int64_t>(std::stod(rule->Value));
        hashes[rule->Category] *= rule->Date.GetYear();
        hashes[rule->Category] /= rule->Date.GetMonth();
        hashes[rule->Category] -= rule->Date.GetDay();
    }
    int64_t hash = 1;
    for (auto& h : hashes)
    {
        hash *= h.second;
    }

    // Check results
    const int64_t expectedHash = -198114973815180800;
    if (hash != expectedHash)
    {
        Utils::PrintError(fmt::format("Computed hash {} does not match expeted hash {} !", hash, expectedHash));
        success = false;
    }
    const int64_t expectedSum = -19886;
    if (sum != expectedSum)
    {
        Utils::PrintError(fmt::format("Computed sum {} does not match expeted sum {} !", sum, expectedSum));
        success = false;
    }
    const int64_t expectedAssignedSize = 56;
    if (database->Assigned.size() != expectedAssignedSize)
    {
        Utils::PrintError(fmt::format("Number of assigned items {} does not match expeted count {} !",
                                      database->Assigned.size(), expectedAssignedSize));
        success = false;
    }
    const int64_t expectedUnassignedSize = 5;
    if (database->Unassigned.size() != expectedUnassignedSize)
    {
        Utils::PrintError(fmt::format("Number of unassigned items {} does not match expeted count {} !",
                                      database->Unassigned.size(), expectedUnassignedSize));
        success = false;
    }
    const int64_t expectedIssuesSize = 5;
    if (database->Issues.size() != expectedIssuesSize)
    {
        Utils::PrintError(fmt::format("Number of issues {} does not match expeted count {} !",
                                      database->Issues.size(), expectedIssuesSize));
        success = false;
    }
    const int64_t expectedRulesSize = 8;
    if (database->Rules.size() != expectedRulesSize)
    {
        Utils::PrintError(fmt::format("Number of found rules {} does not match expeted count {} !",
                                      database->Rules.size(), expectedRulesSize));
        success = false;
    }

    return success;
}

int main()
{
    int result = 0;
    std::set_terminate(Utils::TerminationHandler);
    try
    {
        Utils::PrintInfo(fmt::format("hokee-test version {}", PROJECT_VERSION));

        result += runTest("RuleTest", RuleTest) ? 100 : 101;
        result += runTest("FormatTest", FormatTest) ? 100 : 101;
        result += runTest("HtmlTest", HtmlTest) ? 100 : 101;
    }
    catch (const UserException& e)
    {
        Utils::TerminationHandler(e);
    }
    catch (const std::exception& e)
    {
        Utils::TerminationHandler(e);
    }
    catch (...)
    {
        Utils::TerminationHandler();
    }

    int tests = result / 100;
    int failed = result - tests * 100;
    if (failed == 0)
    {
        Utils::PrintInfo(" => ALL TEST PASSED");
    }
    else
    {
        Utils::PrintError(fmt::format("=> {}/{} TESTS FAILED", failed, tests));
    }

    return result;
}
