#include "Application.h"
#include "InternalException.h"
#include "Utils.h"
#include "Filesystem.h"

#include <fmt/format.h>

#include <exception>
#include <iostream>
#include <map>

using namespace hokee;

void TerminationHandler()
{
    Utils::PrintError("!!! Unhandled exception !!!");

    // Try to decode exception message
    try
    {
        if (std::current_exception())
        {
            std::rethrow_exception(std::current_exception());
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "=> " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "=> ???" << std::endl;
    }

    std::cerr << std::endl;

    std::exit(-3);
}

void TerminationHandler(const std::exception& e)
{
    Utils::PrintError("!!! Caught exception !!!");
    std::cerr << "=> " << e.what() << std::endl;
    std::cerr << std::endl;

    std::exit(-2);
}

void TerminationHandler(const UserException& e)
{
    Utils::PrintError(e.what());
    std::abort();
}

int main(int /*unused*/, const char* argv[])
{
    std::set_terminate(&TerminationHandler);

    bool success = true;
    try
    {
        int testArgc = 2;
        Settings config;
        std::string configPath = "../test_data/rules_add_test/rules_add_test.ini";
        config.SetAddRules(false);
        config.SetUpdateRules(false);
        config.SetRuleSetFile("rules_add_test.csv");
        config.Save(configPath);
        const char* testArgv[] = {argv[0], configPath.c_str(), nullptr};
        auto app = std::make_unique<Application>(testArgc, testArgv);
        std::unique_ptr<CsvDatabase> database = app->Run(true);

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
        database = app2.Run(true);

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
        TerminationHandler(e);
    }
    catch (const std::exception& e)
    {
        TerminationHandler(e);
    }
    catch (...)
    {
        TerminationHandler();
    }

    return success ? 0 : -1;
}
