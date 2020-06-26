#pragma once

#include "Utils.h"

#include <unordered_map>

namespace hokee
{

class CsvSettings
{
    std::unordered_map<std::string, std::string> settings{};

  public:
    CsvSettings(const fs::path& file);
    ~CsvSettings() = default;

    CsvSettings(const CsvSettings&) = delete;
    CsvSettings& operator=(const CsvSettings&) = delete;
    CsvSettings(CsvSettings&&) = delete;
    CsvSettings& operator=(CsvSettings&&) = delete;

};

} // namespace hokee