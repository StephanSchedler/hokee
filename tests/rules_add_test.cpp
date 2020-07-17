#include "Application.h"
#include "InternalException.h"
#include "Utils.h"
#include "Filesystem.h"

#include <fmt/format.h>

#include <exception>
#include <iostream>
#include <map>

using namespace hokee;

int main(int /*unused*/, const char* argv[])
{
    std::set_terminate([]{Utils::TerminationHandler(false);});

    bool success = true;
    try
    {
        Settings config;
        std::string configPath = "../test_data/rules_add_test/rules_add_test.ini";
        config.SetAddRules(false);
        config.SetUpdateRules(false);
        config.SetRuleSetFile("rules_add_test.csv");
        config.Save(configPath);
        const char* testArgv[] = {argv[0], "-i", "-b", configPath.c_str(), nullptr};
        int testArgc = sizeof(testArgv)/sizeof(testArgv[0]) - 1;
        auto app = std::make_unique<Application>(testArgc, testArgv);
        std::unique_ptr<CsvDatabase> database = app->Run();

        // Check results
        uint64_t expectedAssignedSize = 0;
        if (database->Assigned.size() != expectedAssignedSize)
        {
            Utils::PrintError(fmt::format("Number of assigned items {} does not match expeted count {} !",
                                          database->Unassigned.size(), expectedAssignedSize));
            success = false;
        }
        uint64_t expectedUnassignedSize = database->Data.size();
        if (database->Unassigned.size() != expectedUnassignedSize)
        {
            Utils::PrintError(fmt::format("Number of unassigned items {} does not match expeted count {} !",
                                          database->Unassigned.size(), expectedUnassignedSize));
            success = false;
        }
        uint64_t expectedIssuesSize = 0;
        if (database->Issues.size() != expectedIssuesSize)
        {
            Utils::PrintError(fmt::format("Number of issues {} does not match expeted count {} !",
                                          database->Issues.size(), expectedIssuesSize));
            success = false;
        }

        // Add rules        
        config.SetAddRules(true);
        fs::path csvFile = fs::absolute("../test_data/rules_add_test/input_add_test.csv");
#ifdef _MSC_VER
        config.SetEditor(fmt::format("copy /Y {}", csvFile.make_preferred().string()));
#else
        config.SetEditor(fmt::format("cp -f {}", csvFile.string()));
#endif
        config.Save(configPath);
        Application app2(testArgc, testArgv);
        database = app2.Run();

        // Check result
        expectedAssignedSize = database->Data.size();
        if (database->Assigned.size() != expectedAssignedSize)
        {
            Utils::PrintError(fmt::format("Number of assigned items {} does not match expeted count {} !",
                                          database->Unassigned.size(), expectedAssignedSize));
            success = false;
        }
        expectedUnassignedSize = 0;
        if (database->Unassigned.size() != expectedUnassignedSize)
        {
            Utils::PrintError(fmt::format("Number of unassigned items {} does not match expeted count {} !",
                                          database->Unassigned.size(), expectedUnassignedSize));
            success = false;
        }
        expectedIssuesSize = 0;
        if (database->Issues.size() != expectedIssuesSize)
        {
            Utils::PrintError(fmt::format("Number of issues {} does not match expeted count {} !",
                                          database->Issues.size(), expectedIssuesSize));
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
        Utils::TerminationHandler(e, false);
    }
    catch (const std::exception& e)
    {
        Utils::TerminationHandler(e, false);
    }
    catch (...)
    {
        Utils::TerminationHandler(false);
    }

    return success ? 0 : -1;
}
