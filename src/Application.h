#pragma once

#include "csv/CsvDatabase.h"
#include "Settings.h"

#include <cstdint>
#include <memory>


namespace hokee
{
class Application
{
    Settings _config;
    fs::path _inputDirectory{};
    fs::path _tempDirectory{};
    fs::path _ruleSetFile{};

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