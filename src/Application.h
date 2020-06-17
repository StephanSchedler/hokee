#pragma once

#include "csv/CsvDatabase.h"

#include <cstdint>
#include <memory>


namespace hokee
{
class Application
{
    fs::path _inputDirectory{};
    fs::path _outputDirectory{};
    fs::path _ruleSetFile{};
    std::unique_ptr<CsvDatabase> _csvDatabase = nullptr;

  public:
    Application(int argc, const char* argv[], const fs::path& inputDirectory, const fs::path& outputDirectory, const fs::path& ruleSetFile);
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    std::unique_ptr<CsvDatabase> Run(bool batchMode, bool defaultAddRules, bool defaultUpdateRules, bool defaultGenerateReport, const std::string& editor);
};

} // namespace hokee