#include "Application.h"
#include "InternalException.h"
#include "Utils.h"

#include <fmt/format.h>

#include <exception>
#include <iostream>
#include <map>

using namespace hokee;

int main(int /*unused*/, const char* argv[])
{
    std::set_terminate(Utils::TerminationHandler);

    bool success = true;
    try
    {
        Settings config;
        std::string configPath = "../test_data/rules_match_test/rules_match_test.ini";
        config.SetRuleSetFile("rules_match_test.csv");
        config.Save(configPath);
        const char* testArgv[] = {argv[0], "-i", "-b", configPath.c_str(), nullptr};
        int testArgc = sizeof(testArgv)/sizeof(testArgv[0]) - 1;
        auto app = std::make_unique<Application>(testArgc, testArgv);
        std::unique_ptr<CsvDatabase> database = app->Run();

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
            Utils::PrintError(
                fmt::format("Computed hash {} does not match expeted hash {} !", hash, expectedHash));
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

        if(success)
        {
            Utils::PrintInfo("TEST PASSED");
        }
        else
        {
            Utils::PrintInfo("TEST FAILED");
        }
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

    return success ? 0 : -1;
}
