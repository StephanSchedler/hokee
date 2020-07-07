#pragma once

#include "Utils.h"

#include <unordered_map>

namespace hokee
{
class CsvConfig
{
    fs::path _file{};
    std::unordered_map<std::string, std::string> _config{};

  protected:
    void SetString(const std::string& key, const std::string& value);
    std::string GetString(const std::string& key);
    bool GetBool(const std::string& key);

    CsvConfig() = default;
    CsvConfig(const fs::path& file);
    virtual ~CsvConfig() = default;

    CsvConfig(const CsvConfig&) = delete;
    CsvConfig& operator=(const CsvConfig&) = delete;
    CsvConfig(CsvConfig&&) = delete;
    CsvConfig& operator=(CsvConfig&&) = default;

  public:
    void Save(const fs::path& path);
};

} // namespace hokee