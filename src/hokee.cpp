#include "Application.h"
#include "InternalException.h"
#include "Utils.h"

#include <exception>
#include <iostream>

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

    if (std::system("pause"))
    {
        Utils::PrintError("Could not pause.");
    }
    std::abort();
}

void TerminationHandler(const std::exception& e)
{
    Utils::PrintError("!!! Caught exception !!!");
    std::cerr << "=> " << e.what() << std::endl;
    std::cerr << std::endl;

    if (std::system("pause"))
    {
        Utils::PrintError("Could not pause.");
    }
    std::abort();
}

void TerminationHandler(const UserException& e)
{
    Utils::PrintError(e.what());

    if (std::system("pause"))
    {
        Utils::PrintError("Could not pause.");
    }
    std::abort();
}

int main(int argc, const char* argv[])
{
    std::set_terminate(&TerminationHandler);

    try
    {
        const fs::path inputDirectory = "../input/";
        const fs::path outputDirectory = "../output/";
        const fs::path ruleSetFile = "../input/rules.csv";
        Application app(argc, argv, inputDirectory, outputDirectory, ruleSetFile);
        
        const bool defaultAddRules = true;
        const bool defaultUpdateRules = true;
        const bool defaultGenerateReport = true;
        const std::string editor = "code --wait";
        std::unique_ptr<CsvDatabase> database = app.Run(false, defaultAddRules, defaultUpdateRules, defaultGenerateReport, editor);


        Utils::PrintInfo("DONE");
        if (std::system("pause"))
        {
            Utils::PrintError("Could not pause.");
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

    return 0;
}