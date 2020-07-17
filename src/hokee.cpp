#include "Application.h"
#include "InternalException.h"
#include "Utils.h"

#include <exception>
#include <iostream>

using namespace hokee;

int main(int argc, const char* argv[])
{
    std::set_terminate([]{Utils::TerminationHandler(true);});

    try
    {
        Application app(argc, argv);
        std::unique_ptr<CsvDatabase> database = app.Run();

        Utils::PrintInfo("DONE");
        if (std::system("pause"))
        {
            Utils::PrintError("Could not pause.");
        }
    }
    catch (const UserException& e)
    {
        Utils::TerminationHandler(e, true);
    }
    catch (const std::exception& e)
    {
        Utils::TerminationHandler(e, true);
    }
    catch (...)
    {
        Utils::TerminationHandler(true);
    }

    return 0;
}
