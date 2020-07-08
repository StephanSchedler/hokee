#include "csv/CsvConfig.h"
#include "InternalException.h"
#include "UserException.h"
#include "Utils.h"
#include "csv/CsvFormat.h"
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

void CsvConfig::SetStrings(const std::string& key, const std::vector<std::string>& value)
{
    std::string valueString = "";
    if (!value.empty())
    {
        valueString += value[0];
    }
    for (int i = 1; i < value.size(); ++i)
    {
        valueString += ";" + value[i];
    }
    SetString(key, valueString);
}

void CsvConfig::SetBool(const std::string& key, bool value)
{
    SetString(key, value ? "true" : "false");
}

void CsvConfig::SetChar(const std::string& key, char value)
{
    SetString(key, std::string(1, value));
}

void CsvConfig::SetInt(const std::string& key, int value)
{
    SetString(key, std::to_string(value));
}

const std::string CsvConfig::GetString(const std::string& key) const
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

int CsvConfig::GetInt(const std::string& key) const
{
    std::string value = GetString(key);
    int intValue = 0;
    try
    {
        intValue = std::stoi(value);
    }
    catch (const std::exception& e)
    {
        throw UserException(fmt::format("Could not convert '{}' in {} to 'int'. ({})", value, _file.string(), e.what()));
    }
    return intValue;
}

const std::vector<std::string> CsvConfig::GetStrings(const std::string& key) const
{
    std::string value = GetString(key);
    return Utils::SplitLine(value, ';');
}

char CsvConfig::GetChar(const std::string& key) const
{
    std::string value = GetString(key);
    if (value.size() != 1)
    {
        throw UserException(
            fmt::format("Delimiter property '{}' in {} must have length 1.", value, _file.string()));
    }

    return value[0];
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

const CsvFormat CsvConfig::GetFormat()
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