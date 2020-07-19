#include "Application.h"
#include "HtmlGenerator.h"
#include "HttpServer.h"
#include "InternalException.h"
#include "Utils.h"
#include "csv/CsvDatabase.h"
#include "csv/CsvItem.h"
#include "csv/CsvParser.h"
#include "csv/CsvWriter.h"
#include "hokee.h"

#include <chrono>
#include <fmt/format.h>

#include <memory>
#include <stdlib.h>
#include <thread>
#include <unordered_map>

namespace hokee
{
Application::Application(int argc, const char* argv[])
{
    // parse commandline arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);

        if (arg == "--help" || arg == "-h" || arg == "-help" || arg == "-?")
        {
            Utils::PrintInfo(fmt::format("{} version {}", fs::path(argv[0]).filename().string(), PROJECT_VERSION));
            Utils::PrintInfo(fmt::format("{} ({})", PROJECT_DESCRIPTION, PROJECT_HOMEPAGE_URL));
            Utils::PrintInfo("");
            Utils::PrintInfo("usage: hokee [-i|--interactive [-b|--batch]] [path] | [-h|--help] | [-v|--version]");
            Utils::PrintInfo(
                "  -b,--batch      - Batch mode that does not ask questions for interactive mode, but");
            Utils::PrintInfo("                    uses default values from settings file.");
            Utils::PrintInfo("  -h,--help       - Show this help.");
            Utils::PrintInfo(
                "  -i,--iteractive - Interactive commandline mode that does not start the webserver.");
            Utils::PrintInfo("                    If there are issues or unassigned items, user can fix them.");
            Utils::PrintInfo("  -v,--version    - Show this help.");
            Utils::PrintInfo("  path            - Path to config file.");
            Utils::PrintInfo("                    (default: ~/hokee/hokee.ini)");
            std::exit(EXIT_SUCCESS);
        }
        else if (arg == "-v" || arg == "-version" || arg == "--version")
        {
            Utils::PrintInfo(fmt::format("{} version {}", fs::path(argv[0]).filename().string(), PROJECT_VERSION));
            Utils::PrintInfo(fmt::format("{} ({})", PROJECT_DESCRIPTION, PROJECT_HOMEPAGE_URL));
            std::exit(EXIT_SUCCESS);
        }
        else if (arg == "-b" || arg == "-batch" || arg == "--batch")
        {
            _batchMode = true;
        }
        else if (arg == "-i" || arg == "-interactive" || arg == "--interactive")
        {
            _interactiveMode = true;
        }
        else
        {
            _configFile = fs::absolute(arg);
            if (!fs::exists(_configFile))
            {
                throw UserException(fmt::format("Could not find config file '{}'.", _configFile.string()));
            }
        }
    }

    if (_configFile.empty())
    {
        _configFile = fs::absolute(Utils::GetHomePath() / "hokee" / "hokee.ini");
        if (!fs::exists(_configFile))
        {
            Utils::PrintWarning(
                fmt::format("Could not find config file '{}'. Create new config file.", _configFile.string()));
            if (!fs::exists(_configFile.parent_path()))
            {
                fs::create_directories(_configFile.parent_path());
            }

            Settings config;
            config.Save(_configFile);
        }
    }

    // Read settings
    _config = Settings(_configFile);
    if (_config.GetInputDirectory().is_absolute())
    {
        _inputDirectory = _config.GetInputDirectory();
    }
    else
    {
        _inputDirectory = _configFile.parent_path() / _config.GetInputDirectory();
    }
    if (_config.GetRuleSetFile().is_absolute())
    {
        _ruleSetFile = _config.GetRuleSetFile();
    }
    else
    {
        _ruleSetFile = _configFile.parent_path() / _config.GetRuleSetFile();
    }

    // Detect Temporary Directory
#ifdef _MSC_VER
    _tempDirectory = Utils::GetEnv("TEMP");
#elif __APPLE__
    _tempDirectory = "/var/tmp";
#else
    _tempDirectory = "/tmp";
#endif
    _tempDirectory = _tempDirectory / "hokee";
    if (!fs::exists(_tempDirectory))
    {
        fs::create_directories(_tempDirectory);
    }

    if (!fs::exists(_inputDirectory))
    {
        fs::create_directories(_inputDirectory);        
        Utils::PrintWarning(
            fmt::format("Could not find input directory '{}'. Create new one.", _inputDirectory.string()));
    }
    if (!fs::exists(_ruleSetFile))
    {
        throw UserException(fmt::format("Ruleset file {} does not exits.", fs::absolute(_ruleSetFile).string()));
    }
}

std::unique_ptr<CsvDatabase> Application::Run()
{
    if (!_interactiveMode)
    {
        Utils::PrintInfo("Start HttpServer...");
        std::thread serverThread([&] {
            try
            {
                HttpServer httpServer(_inputDirectory, _ruleSetFile, _configFile, _config.GetEditor(), _config.GetExplorer());
                httpServer.Run();
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
        });

        Utils::PrintInfo("Open browser...");
        std::thread browserThread([config = _config] {
            try
            {
                if (std::system(fmt::format("{} http://localhost", config.GetBrowser()).c_str()) < 0)
                {
                    throw UserException("Could not open result.");
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
        });

        Utils::PrintTrace("Wait for server to exit");
        serverThread.join();

        Utils::PrintTrace("Wait for browser to exit");
        browserThread.join();
    }
    else
    {
        Utils::PrintInfo("Parse CSV files...");

        auto csvDatabase = std::make_unique<CsvDatabase>();
        int maxIterations = 10;
        bool restart = false;
        do
        {
            restart = false;
            if (--maxIterations <= 0)
            {
                throw InternalException(__FILE__, __LINE__, "Infinte loop in batchMode.");
            }

            csvDatabase->Load(_inputDirectory, _ruleSetFile);

            Utils::PrintInfo(fmt::format("Found {} items (assigned: {}, unassigned: {}, issues: {})",
                                         csvDatabase->Data.size(), csvDatabase->Assigned.size(),
                                         csvDatabase->Unassigned.size(), csvDatabase->Issues.size()));

            if (csvDatabase->Unassigned.size() > 0)
            {
                bool defaultAddRules = _config.GetAddRules();
                if (Utils::AskYesNoQuestion("Add rules?", defaultAddRules, _batchMode))
                {
                    std::string editor = _config.GetEditor();
                    csvDatabase->AddRules(_ruleSetFile, _tempDirectory, editor);

                    restart = true;
                    continue;
                }
            }

            if (csvDatabase->Issues.size() > 0 || csvDatabase->Unassigned.size() > 0)
            {
                bool defaultUpdateRules = _config.GetUpdateRules();
                if (Utils::AskYesNoQuestion("Update rules & categories?", defaultUpdateRules, _batchMode))
                {
                    std::string editor = _config.GetEditor();
                    Utils::EditFile(_ruleSetFile, editor);
                    restart = true;
                    continue;
                }
            }
        } while (restart);

        return csvDatabase;
    }
    return nullptr;
}
} // namespace hokee