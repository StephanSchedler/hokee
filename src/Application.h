#pragma once

#include "Settings.h"
#include "csv/CsvDatabase.h"

#include <cstdint>
#include <memory>

namespace hokee
{
class Application
{
    bool _supportMode{false};
    Settings _config;
    fs::path _inputDirectory{};
    fs::path _ruleSetFile{};
    fs::path _configFile{};

    void ReadSettings();

  public:
    Application(int argc, const char* argv[]);
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void Run();
    std::unique_ptr<CsvDatabase> RunBatch();
};

} // namespace hokee