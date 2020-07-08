#pragma once

#include "csv/CsvConfig.h"

namespace hokee
{
class Settings final : public CsvConfig
{
  public:
    Settings();
    Settings(const fs::path& file);

    Settings(const Settings&) = default;
    Settings& operator=(const Settings&) = default;
    Settings(Settings&&) = default;
    Settings& operator=(Settings&&) = default;

    const fs::path GetInputDirectory() const;
    const fs::path GetOutputDirectory() const;
    const fs::path GetRuleSetFile() const;
    const std::string GetEditor() const;
    bool GetAddRules() const;
    bool GetUpdateRules() const;
    bool GetGenerateReport() const;

    void SetInputDirectory(const fs::path& value);
    void SetOutputDirectory(const fs::path& value);
    void SetRuleSetFile(const fs::path& value);
    void SetEditor(const std::string& value);
    void SetAddRules(bool value);
    void SetUpdateRules(bool value);
    void SetGenerateReport(bool value);
};

} // namespace hokee