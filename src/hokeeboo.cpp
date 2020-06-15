#include "Application.h"
#include "CustomException.h"
#include "Utils.h"

#include <exception>
#include <iostream>

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

int main()
{
    Utils::PrintInfo("Set termination handler...");
    std::set_terminate(&TerminationHandler);

    try
    {
        const fs::path inputDirectory = "../input/";
        const fs::path outputDirectory = "../output/";
        Application app(inputDirectory, outputDirectory);
        app.Run();

        Utils::PrintInfo("DONE");
        if (std::system("pause"))
        {
            Utils::PrintError("Could not pause.");
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

    return 0;
}
