#include "csv/CsvFormat.h"
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
    CsvParser csv(file, CsvConfig::GetFormat());

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

std::string CsvConfig::GetString(const std::string& key) const
{
    auto item = _config.find(key);
    if (item == _config.end())
    {
        throw UserException(
            fmt::format("Missing property. Could not find property '{}' in {}", key, _file.string()));
    }
    Utils::PrintInfo(fmt::format("Read setting: {}={}", key, item->second));
    return item->second;
}

bool CsvConfig::GetBool(const std::string& key) const
{
    return GetString(key) == "true";
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

CsvFormat GetFormat()
{
    CsvFormat format;
    format.SetColumnNames({"name", "value"});
    format.SetHasHeader(false);
    format.SetIgnoreLines(0);
    format.SetHasDoubleQuotes(false);
    format.SetHasTrailingDelimiter(false);
    format.SetDelimiter('=');
    format.SetCategory(0);
    format.SetPayerPayee(-1);
    format.SetPayer(-1);
    format.SetPayee(-1);
    format.SetDescription(1);
    format.SetType(-1);
    format.SetDate(-1);
    format.SetAccount(-1);
    format.SetValue(-1);
    return format;
}

} // namespace hokee