#include "Application.h"
#include "InternalException.h"
#include "Utils.h"

#include <exception>
#include <iostream>

using namespace hokee;

int main(int argc, const char* argv[])
{
    std::set_terminate(Utils::TerminationHandler);

    try
    {
        Application app(argc, argv);
        std::unique_ptr<CsvDatabase> database = app.Run();

        Utils::PrintInfo("DONE");
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

    return 0;
}
