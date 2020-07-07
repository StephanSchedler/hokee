#include "Application.h"
#include "InternalException.h"
#include "ReportGenerator.h"
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
    fs::path configPath = "";
    if (argc == 1)
    {
        configPath = fs::absolute(Utils::GetHomePath() / "hokee" / "hokee.ini");
        if (!fs::exists(configPath))
        {
            Utils::PrintWarning(fmt::format("Could not find config file '{}'. Create new config file.",
                                            configPath.string()));
            if (!fs::exists(configPath.parent_path()))
            {
                fs::create_directories(configPath.parent_path());
            }

            CsvConfig config;
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
    _config = CsvConfig(configPath);

    _inputDirectory = configPath.parent_path() / _config.GetInputDirectory();
    _outputDirectory = configPath.parent_path() / _config.GetOutputDirectory();
    _ruleSetFile = configPath.parent_path() / _config.GetRuleSetFile();

    if (!fs::exists(_inputDirectory))
    {
        throw UserException(
            fmt::format("Input directory {} does not exits.", fs::absolute(_inputDirectory).string()));
    }
    if (!fs::exists(_outputDirectory))
    {
        Utils::PrintWarning(fmt::format("Output directory {} does not exits. Create new one.",
                                        fs::absolute(_outputDirectory).string()));
        fs::create_directories(_outputDirectory);
    }
    if (!fs::exists(_ruleSetFile))
    {
        throw UserException(fmt::format("Ruleset file {} does not exits.", fs::absolute(_ruleSetFile).string()));
    }
}

std::unique_ptr<CsvDatabase> Application::Run(bool batchMode)
{
    const fs::path indexHtml = _outputDirectory / "index.html";
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

        _csvDatabase = std::make_unique<CsvDatabase>(_inputDirectory, _ruleSetFile);

        Utils::PrintInfo("");
        Utils::PrintInfo(fmt::format("Found {} items (assigned: {}, unassigned: {}, issues: {})",
                                     _csvDatabase->Data.size(), _csvDatabase->Assigned.size(),
                                     _csvDatabase->Unassigned.size(), _csvDatabase->Issues.size()));

        if (_csvDatabase->Unassigned.size() > 0)
        {
            bool defaultAddRules = _config.GetAddRules();
            if (Utils::AskYesNoQuestion("Add rules?", defaultAddRules, batchMode))
            {
                std::string editor = _config.GetEditor();
                _csvDatabase->AddRules(_ruleSetFile, _outputDirectory, editor);

                restart = true;
                continue;
            }
        }

        if (_csvDatabase->Issues.size() > 0 || _csvDatabase->Unassigned.size() > 0)
        {
            bool defaultUpdateRules = _config.GetUpdateRules();
            if (Utils::AskYesNoQuestion("Update rules & categories?", defaultUpdateRules, batchMode))
            {
                std::string editor = _config.GetEditor();
                _csvDatabase->UpdateRules(_ruleSetFile, editor);
                restart = true;
                continue;
            }
        }
    } while (restart);

    bool defaultGenerateReport = _config.GetGenerateReport();
    if (Utils::AskYesNoQuestion("Generate full report?", defaultGenerateReport, batchMode))
    {
        ReportGenerator reportGenerator(_csvDatabase.get());
        reportGenerator.Write(_outputDirectory);
    }

    if (!batchMode)
    {
        Utils::PrintInfo("");
        Utils::PrintInfo("Open report...");
        if (std::system(fs::absolute(indexHtml).string().c_str()) < 0)
        {
            throw UserException("Could not open report.");
        }
    }

    return std::move(_csvDatabase);
}
} // namespace hokee