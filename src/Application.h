#pragma once

#include "csv/CsvDatabase.h"
#include "csv/CsvConfig.h"

#include <cstdint>
#include <memory>


namespace hokee
{
class Application
{
    CsvConfig _config;
    fs::path _inputDirectory{};
    fs::path _outputDirectory{};
    fs::path _ruleSetFile{};
    std::unique_ptr<CsvDatabase> _csvDatabase = nullptr;

  public:
    Application(int argc, const char* argv[]);
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    std::unique_ptr<CsvDatabase> Run(bool batchMode);
};

} // namespace hokee