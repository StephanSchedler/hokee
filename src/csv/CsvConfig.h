#pragma once

#include "../Filesystem.h"

#include <unordered_map>

namespace hokee
{
class CsvFormat;

class CsvConfig
{
    fs::path _file{};
    std::unordered_map<std::string, std::string> _config{};
    static CsvFormat GetFormat();

  protected:
    void SetString(const std::string& key, const std::string& value);
    std::string GetString(const std::string& key) const;
    bool GetBool(const std::string& key) const;

    CsvConfig() = default;
    CsvConfig(const fs::path& file);
    virtual ~CsvConfig() = default;

    CsvConfig(const CsvConfig&) = default;
    CsvConfig& operator=(const CsvConfig&) = default;
    CsvConfig(CsvConfig&&) = default;
    CsvConfig& operator=(CsvConfig&&) = default;

  public:
    void Save(const fs::path& path);
};

} // namespace hokee