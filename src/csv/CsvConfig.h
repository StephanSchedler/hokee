#pragma once

#include "../Filesystem.h"

#include <unordered_map>
#include <vector>

namespace hokee
{
class CsvFormat;

class CsvConfig
{
    fs::path _file{"???"};
    std::unordered_map<std::string, std::string> _config{};
    static const CsvFormat GetFormat();

  protected:

    const std::string GetString(const std::string& key) const;
    const std::vector<std::string> GetStrings(const std::string& key) const;
    bool GetBool(const std::string& key) const;
    char GetChar(const std::string& key) const;
    int GetInt(const std::string& key) const;

    void SetString(const std::string& key, const std::string& value);
    void SetStrings(const std::string& key, const std::vector<std::string>& value);
    void SetBool(const std::string& key, bool value);
    void SetChar(const std::string& key, char value);
    void SetInt(const std::string& key, int value);

    CsvConfig() = default;
    CsvConfig(const std::unordered_map<std::string, std::string>& config, const fs::path& file);
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