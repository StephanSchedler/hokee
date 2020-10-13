#pragma once

#include "csv/CsvConfig.h"

namespace hokee
{
class Settings final : public CsvConfig
{
  public:
    Settings();
    explicit Settings(const fs::path& file);

    Settings(const Settings&) = default;
    Settings& operator=(const Settings&) = default;
    Settings(Settings&&) = default;
    Settings& operator=(Settings&&) = default;

    const fs::path GetInputDirectory() const;
    const fs::path GetRuleSetFile() const;
    const std::string GetExplorer() const;
    const std::string GetBrowser() const;

    void SetInputDirectory(const fs::path& value);
    void SetRuleSetFile(const fs::path& value);
    void SetExplorer(const std::string& value);
    void SetBrowser(const std::string& value);
};

} // namespace hokee