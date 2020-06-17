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
Application::Application(int argc, const char* argv[], const fs::path& inputDirectory,
                         const fs::path& outputDirectory, const fs::path& ruleSetFile)
    : _inputDirectory{inputDirectory}
    , _outputDirectory{outputDirectory}
    , _ruleSetFile{ruleSetFile}
{
    if (argc > 1)
    {
        Utils::PrintInfo(fmt::format("{} version {}", fs::path(argv[0]).filename().string(), PROJECT_VERSION));
        Utils::PrintInfo(fmt::format("{} ({})", PROJECT_DESCRIPTION, PROJECT_HOMEPAGE_URL));
        std::exit(EXIT_SUCCESS);
    }

    if (!fs::exists(inputDirectory))
    {
        throw UserException(
            fmt::format("Input directory {} does not exits.", fs::absolute(inputDirectory).string()));
    }
    if (!fs::exists(outputDirectory))
    {
        Utils::PrintWarning(
            fmt::format("Output directory {} does not exits. Create new one.", fs::absolute(outputDirectory).string()));
        fs::create_directories(outputDirectory);
    }
    if (!fs::exists(ruleSetFile))
    {
        throw UserException(fmt::format("Ruleset file {} does not exits.", fs::absolute(ruleSetFile).string()));
    }
}

std::unique_ptr<CsvDatabase> Application::Run(bool batchMode, bool defaultAddRules, bool defaultUpdateRules,
                                              bool defaultGenerateReport, const std::string& editor)
{
    const fs::path indexHtml = _outputDirectory / "index.html";
    Utils::PrintInfo("Parse CSV files...");

    int maxIterations = 1000;
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
            if (Utils::AskYesNoQuestion("Add rules?", defaultAddRules, batchMode))
            {
                _csvDatabase->AddRules(_ruleSetFile, _outputDirectory, editor);

                restart = true;
                continue;
            }
        }

        if (_csvDatabase->Issues.size() > 0 || _csvDatabase->Unassigned.size() > 0)
        {
            if (Utils::AskYesNoQuestion("Update rules & categories?", defaultUpdateRules, batchMode))
            {
                _csvDatabase->UpdateRules(_ruleSetFile, editor);
                restart = true;
                continue;
            }
        }
    } while (restart);

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