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
    fs::path configPath = "";
    if (argc == 1)
    {
        configPath = fs::absolute(Utils::GetHomePath() / "hokee" / "hokee.ini");
        if (!fs::exists(configPath))
        {
            Utils::PrintWarning(
                fmt::format("Could not find config file '{}'. Create new config file.", configPath.string()));
            if (!fs::exists(configPath.parent_path()))
            {
                fs::create_directories(configPath.parent_path());
            }

            Settings config;
            config.Save(configPath);
        }
    }
    else if (argc == 2)
    {
        configPath = fs::absolute(argv[1]);
        if (!fs::exists(configPath))
        {
            throw UserException(fmt::format("Could not find config file '{}'.", configPath.string()));
        }
    }
    else
    {
        Utils::PrintInfo(fmt::format("{} version {}", fs::path(argv[0]).filename().string(), PROJECT_VERSION));
        Utils::PrintInfo(fmt::format("{} ({})", PROJECT_DESCRIPTION, PROJECT_HOMEPAGE_URL));
        Utils::PrintInfo("");
        Utils::PrintInfo("usage: hokee [path]");
        Utils::PrintInfo("  path - path to config file");
        Utils::PrintInfo("         (default: ~/hokee/hokee.ini)");
        std::exit(EXIT_SUCCESS);
    }

    // Read settings
    _config = Settings(configPath);
    if (_config.GetInputDirectory().is_absolute())
    {
        _inputDirectory = _config.GetInputDirectory();
    }
    else
    {
        _inputDirectory = configPath.parent_path() / _config.GetInputDirectory();
    }
    if (_config.GetRuleSetFile().is_absolute())
    {
        _ruleSetFile = _config.GetRuleSetFile();
    }
    else
    {
        _ruleSetFile = configPath.parent_path() / _config.GetRuleSetFile();
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
        throw UserException(
            fmt::format("Input directory {} does not exits.", fs::absolute(_inputDirectory).string()));
    }
    if (!fs::exists(_ruleSetFile))
    {
        throw UserException(fmt::format("Ruleset file {} does not exits.", fs::absolute(_ruleSetFile).string()));
    }
}

std::unique_ptr<CsvDatabase> Application::Run(bool batchMode)
{
    auto csvDatabase = std::make_unique<CsvDatabase>();
    std::unique_ptr<std::thread> serverThread = nullptr;

    std::unique_lock lock(csvDatabase->ReadLock, std::try_to_lock);
    if (!lock.owns_lock())
    {
        throw UserException("Could not get lock on database!");
    }

    if (!batchMode)
    {
        Utils::PrintInfo("Start HttpServer...");
        serverThread = std::make_unique<std::thread>([&] {
            HttpServer httpServer(csvDatabase.get());
            httpServer.Run();
        });

        Utils::PrintInfo("Open result...");
        if (std::system(fmt::format("{} http://localhost", _config.GetBrowser()).c_str()) < 0)
        {
            throw UserException("Could not open result.");
        }
    }

    Utils::PrintInfo("Parse CSV files...");

    int maxIterations = 10;
    bool restart = false;
    do
    {
        restart = false;
        if (batchMode && --maxIterations <= 0)
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
            if (Utils::AskYesNoQuestion("Add rules?", defaultAddRules, batchMode))
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
            if (Utils::AskYesNoQuestion("Update rules & categories?", defaultUpdateRules, batchMode))
            {
                std::string editor = _config.GetEditor();
                csvDatabase->UpdateRules(_ruleSetFile, editor);
                restart = true;
                continue;
            }
        }
    } while (restart);
    lock.unlock();

    if (serverThread)
    {
        Utils::PrintTrace("Wait for server thread to exit");
        serverThread->join();
    }
    return csvDatabase;
}
} // namespace hokee