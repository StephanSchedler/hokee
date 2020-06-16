#include "Application.h"
#include "CsvDatabase.h"
#include "CsvItem.h"
#include "CsvParser.h"
#include "CsvWriter.h"
#include "CustomException.h"
#include "ReportGenerator.h"
#include "Utils.h"
#include "hokeeboo.h"

#include <chrono>
#include <fmt/format.h>

#include <memory>
#include <stdlib.h>
#include <thread>
#include <unordered_map>

namespace hokeeboo
{
Application::Application(int argc, const char* argv[], const fs::path& inputDirectory,
                         const fs::path& outputDirectory)
    : _inputDirectory{inputDirectory}
    , _outputDirectory{outputDirectory}
{
    if (argc > 1)
    {
        Utils::PrintInfo(fmt::format("{} version {}", fs::path(argv[0]).filename().string(), PROJECT_VERSION));
        Utils::PrintInfo(fmt::format("{} ({})", PROJECT_DESCRIPTION, PROJECT_HOMEPAGE_URL));
        std::exit(EXIT_SUCCESS);
    }
}

std::unique_ptr<CsvDatabase> Application::Run(bool batchMode)
{
    const fs::path indexHtml = _outputDirectory / "index.html";
    const fs::path rulesCsv = _inputDirectory / "rules.csv";

    Utils::PrintInfo("Parse CSV files...");

    bool restart = false;
    do
    {
        restart = false;

        _csvDatabase = std::make_unique<CsvDatabase>(_inputDirectory, rulesCsv);

        Utils::PrintInfo("");
        Utils::PrintInfo(fmt::format("Found {} items (assigned: {}, unassigned: {}, issues: {})",
                                     _csvDatabase->Data.size(), _csvDatabase->Assigned.size(),
                                     _csvDatabase->Unassigned.size(), _csvDatabase->Issues.size()));

        if (_csvDatabase->Issues.size() > 0)
        {
            char answer = batchMode ? 'n' : Utils::AskYesNoQuestion("Fix rules?");
            if (answer == 'Y' || answer == 'y')
            {
                _csvDatabase->FixRules(rulesCsv);
                restart = true;
                continue;
            }
        }

        if (_csvDatabase->Unassigned.size() > 0)
        {
            char answer = batchMode ? 'n' : Utils::AskYesNoQuestion("Add rules?");
            if (answer == 'Y' || answer == 'y')
            {
                _csvDatabase->AddRules(rulesCsv, _outputDirectory);

                restart = true;
                continue;
            }
        }
    } while (restart);

    char answer = batchMode ? 'n' : Utils::AskYesNoQuestion("Generate full report?");
    if (answer == 'Y' || answer == 'y')
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
            throw CustomException(__FILE__, __LINE__, "Could not open report.");
        }
    }

    return std::move(_csvDatabase);
}
} // namespace hokeeboo