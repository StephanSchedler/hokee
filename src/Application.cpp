#include "Application.h"
#include "HttpServer.h"
#include "InternalException.h"
#include "Utils.h"
#include "csv/CsvDatabase.h"
#include "csv/CsvItem.h"
#include "csv/CsvParser.h"
#include "csv/CsvWriter.h"
#include "hokee.h"
#include "html/HtmlGenerator.h"

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
            Utils::PrintInfo(fmt::format("hokee version {}", PROJECT_VERSION));
            Utils::PrintInfo(fmt::format("{} ({})", PROJECT_DESCRIPTION, PROJECT_HOMEPAGE_URL));
            Utils::PrintInfo("");
            Utils::PrintInfo("usage: hokee [options] [path]");
            Utils::PrintInfo("");
            Utils::PrintInfo("  path            - Path to config file.");
            Utils::PrintInfo("                    (default: ~/hokee/hokee.ini)");
            Utils::PrintInfo("  -v,--verbose    - Print trace infos to console.");
            Utils::PrintInfo("  --version       - Show version infos.");
            Utils::PrintInfo("  -h,--help       - Show this help.");
            Utils::PrintInfo("  -s,--support    - Generate anonymized support information.");
            std::exit(EXIT_SUCCESS);
        }
        else if (arg == "-version" || arg == "--version")
        {
            Utils::PrintInfo(fmt::format("hokee version {}", PROJECT_VERSION));
            Utils::PrintInfo(fmt::format("{} ({})", PROJECT_DESCRIPTION, PROJECT_HOMEPAGE_URL));
            std::exit(EXIT_SUCCESS);
        }
        else if (arg == "-s" || arg == "-support" || arg == "--support")
        {
            _supportMode = true;
        }
        else if (arg == "-v" || arg == "-verbose" || arg == "--verbose")
        {
            Utils::SetVerbose(true);
        }
        else
        {
            _configFile = fs::absolute(arg);
            if (!fs::exists(_configFile))
            {
                throw UserException(fmt::format("Could not find config file"), _configFile);
            }
        }
    }

    if (_configFile.empty())
    {
        _configFile = fs::absolute(Utils::GetHomeDir() / "hokee" / "hokee.ini");
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

    if (_supportMode)
    {
        ReadSettings();
        std::string mail = Utils::GenerateSupportMail(_ruleSetFile, _inputDirectory);

        Utils::PrintInfo(fmt::format("Fill the section below and send it to schedler@paderborn.com"));
        std::cout << mail << std::endl;

        std::exit(EXIT_SUCCESS);
    }
}

void Application::ReadSettings()
{
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

    if (!fs::exists(_inputDirectory))
    {
        fs::create_directories(_inputDirectory);
        Utils::PrintWarning(
            fmt::format("Could not find input directory '{}'. Create new one.", _inputDirectory.string()));
    }
    if (!fs::exists(_ruleSetFile))
    {
        Utils::PrintWarning(
            fmt::format("Could not find rules. Create empty rules file {}", _ruleSetFile.string()));
        std::vector<std::string> header{};
        header.push_back("Categories:");
        header.push_back("Categorie1;Categorie2;Ignore!");
        header.push_back("");
        header.push_back("Rules:");

        CsvTable empty;
        empty.SetCsvHeader(std::move(header));
        CsvWriter::Write(_ruleSetFile, empty);
    }
}

std::unique_ptr<CsvDatabase> Application::RunBatch()
{
    ReadSettings();

    auto csvDatabase = std::make_unique<CsvDatabase>();
    csvDatabase->Load(_inputDirectory, _ruleSetFile);

    Utils::PrintInfo(fmt::format("Found {} items (assigned: {}, unassigned: {}, issues: {})",
                                 csvDatabase->Data.size(), csvDatabase->Assigned.size(),
                                 csvDatabase->Unassigned.size(), csvDatabase->Issues.size()));

    return csvDatabase;
}

void Application::Run()
{
    int exitCode = 1;
    while (exitCode > 0)
    {
        try
        {
            ReadSettings();
        }
        catch (UserException& e)
        {
            Utils::PrintError(e.what());
            // Continue, HttpServer will show error page
        }

        Utils::PrintInfo("Open browser...");
        Utils::RunAsync(fmt::format("{} http://localhost", _config.GetBrowser()));

        Utils::PrintInfo("Start HttpServer...");
        HttpServer httpServer(_inputDirectory, _ruleSetFile, _configFile, _config.GetExplorer());
        exitCode = httpServer.Run();
    }
}
} // namespace hokee