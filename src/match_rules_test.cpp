#include "Application.h"
#include "CustomException.h"
#include "Utils.h"

#include <fmt/format.h>

#include <exception>
#include <iostream>
#include <map>

using namespace hokeeboo;

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

int main()
{
    Utils::PrintInfo("Set termination handler...");
    std::set_terminate(&TerminationHandler);

    bool success = true;
    try
    {
        const fs::path inputDirectory = "../../testdata/match_rules_test/input/";
        const fs::path outputDirectory = "../../testdata/match_rules_test/output/";
        Application app(inputDirectory, outputDirectory);
        std::unique_ptr<CsvDatabase> database = app.Run(true);

        // Compute Hash
        std::map<std::string, int64_t> hashes;
        std::vector<std::string> categories;
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
        const int64_t expectedHash = -2080778203735490432;
        if (hash != expectedHash)
        {
            Utils::PrintError(
                fmt::format("Computed hash {} does not match expeted hash {} !", hash, expectedHash));
            success = false;
        }
        const int64_t expectedSum = -652;
        if (sum != expectedSum)
        {
            Utils::PrintError(fmt::format("Computed sum {} does not match expeted sum {} !", sum, expectedSum));
            success = false;
        }
        const int64_t expectedAssignedSize = 25;
        if (database->Assigned.size() != expectedAssignedSize)
        {
            Utils::PrintError(fmt::format("Number of assigned items {} does not match expeted count {} !",
                                          database->Unassigned.size(), expectedAssignedSize));
            success = false;
        }
        const int64_t expectedUnassignedSize = 1;
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
