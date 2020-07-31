#pragma once

#include "Settings.h"
#include "csv/CsvDatabase.h"

#include <cstdint>
#include <memory>

namespace hokee
{
class Application
{
    bool _batchMode{false};
    bool _interactiveMode{false};
    Settings _config;
    fs::path _inputDirectory{};
    fs::path _tempDirectory{};
    fs::path _ruleSetFile{};
    fs::path _configFile{};
    
    void ReadSettings();
    void RunHttpServer();
    std::unique_ptr<CsvDatabase> RunInteractive();

  public:
    Application(int argc, const char* argv[]);
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    std::unique_ptr<CsvDatabase> Run();
};

} // namespace hokee