#include "Application.h"
#include "InternalException.h"
#include "Utils.h"

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

int main(int argc, const char* argv[])
{
    std::set_terminate(&TerminationHandler);

    bool success = true;
    try
    {
        const fs::path inputDirectory = "../../test_data/rules_add_test/";
        const fs::path outputDirectory = "../../test_data/rules_add_test/.output/";
        const fs::path ruleSetFile = "../../test_data/rules_add_test/rules.csv";
        fs::copy_file("../../test_data/rules_add_test/rules_add_test.csv", ruleSetFile ,fs::copy_options::overwrite_existing);
        Application app(argc, argv, inputDirectory, outputDirectory, ruleSetFile);

        bool defaultAddRules = false;
        bool defaultUpdateRules = false;
        bool defaultGenerateReport = false;
        std::string editor = "<NOT_USED>";
        std::unique_ptr<CsvDatabase> database = app.Run(true, defaultAddRules, defaultUpdateRules, defaultGenerateReport, editor);

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
        defaultAddRules = true;
        fs::path inputFile = "../../test_data/rules_add_test/input_add_test.csv";
#ifdef _MSC_VER
        editor = fmt::format("copy /Y \"{}\"", inputFile.make_preferred().string());
#else
        editor = fmt::format("cp -f \"{}\"", inputFile.string());
#endif
        database = app.Run(true, defaultAddRules, defaultUpdateRules, defaultGenerateReport, editor);

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
