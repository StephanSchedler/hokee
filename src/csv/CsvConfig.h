#pragma once

#include "Utils.h"

#include <unordered_map>

namespace hokee
{
class CsvConfig
{
    fs::path _file{};
    std::unordered_map<std::string, std::string> _config{};
    
    void SetString(const std::string& key, const std::string& value);
    std::string GetString(const std::string& key);
    bool GetBool(const std::string& key);

  public:
    CsvConfig();
    CsvConfig(const fs::path& file);
    ~CsvConfig() = default;

    CsvConfig(const CsvConfig&) = delete;
    CsvConfig& operator=(const CsvConfig&) = delete;
    CsvConfig(CsvConfig&&) = delete;
    CsvConfig& operator=(CsvConfig&&) = default;

    void Save(const fs::path& path);
    
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