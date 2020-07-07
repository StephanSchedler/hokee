#include "csv/CsvConfig.h"
#include "UserException.h"
#include "csv/CsvParser.h"
#include "csv/CsvTable.h"

#include <fmt/format.h>

namespace hokee
{
CsvConfig::CsvConfig(const fs::path& file)
    : _file{file}
{
    if (!fs::exists(file))
    {
        throw UserException(fmt::format("File does not exist: {}", file.string()));
    }
    CsvParser csv(file, Utils::GetCsvFormat("Config"));

    CsvTable csvData;
    csv.Load(csvData);

    for (auto& item : csvData)
    {
        _config[item->Category] = item->Description;
    }
}

void CsvConfig::SetString(const std::string& key, const std::string& value)
{
    _config[key] = value;
}

std::string CsvConfig::GetString(const std::string& key)
{
    std::string value = _config[key];
    if (value.empty())
    {
        throw UserException(
            fmt::format("Missing property. Could not find property '{}' in {}", key, _file.string()));
    }
    Utils::PrintInfo(fmt::format("Read setting: {}={} [string]", key, value));
    return value;
}

bool CsvConfig::GetBool(const std::string& key)
{
    std::string value = _config[key];
    if (value.empty())
    {
        throw UserException(
            fmt::format("Missing property. Could not find property '{}' in {}", key, _file.string()));
    }

    bool result = value == "true";
    Utils::PrintInfo(fmt::format("Read setting: {}={} [bool]", key, result));
    return result;
}

void CsvConfig::Save(const fs::path& path)
{
    _file = path;

    std::ofstream output(path);

    for (const auto& value : _config)
    {
        output << fmt::format("{}={}", value.first, value.second) << std::endl;
    }
}

} // namespace hokee