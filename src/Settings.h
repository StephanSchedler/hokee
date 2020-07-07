#pragma once

#include "csv/CsvConfig.h"

namespace hokee
{
class Settings final : public CsvConfig
{
  public:
    Settings();
    Settings(fs::path file);

    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(Settings&&) = default;

    fs::path GetInputDirectory();
    void SetInputDirectory(const fs::path& value);
    fs::path GetOutputDirectory();
    void SetOutputDirectory(const fs::path& value);
    fs::path GetRuleSetFile();
    void SetRuleSetFile(const fs::path& value);
    std::string GetEditor();
    void SetEditor(const std::string& value);
    bool GetAddRules();
    void SetAddRules(bool value);
    bool GetUpdateRules();
    void SetUpdateRules(bool value);
    bool GetGenerateReport();
    void SetGenerateReport(bool value);
};

} // namespace hokee