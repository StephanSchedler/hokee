#pragma once

#include "CsvDatabase.h"

#include <memory>
#include <cstdint>

namespace hokeeboo
{
class Application
{
    fs::path _inputDirectory{};
    fs::path _outputDirectory{};
    std::unique_ptr<CsvDatabase> _csvDatabase = nullptr;
    
  public:
    Application(int argc, const char* argv[], const fs::path& inputDirectory, const fs::path& outputDirectory);
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    std::unique_ptr<CsvDatabase> Run(bool batchMode = false);
};

} // namespace hokeeboo