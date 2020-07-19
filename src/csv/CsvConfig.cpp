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
CsvConfig::CsvConfig(const std::unordered_map<std::string, std::string>& config, const fs::path file)
    : _file{file}
    , _config{config}
{
}

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
    for (size_t i = 1; i < value.size(); ++i)
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
            fmt::format("Missing property. Could not find property '{}' in '{}'", key, _file.string()));
    }
    Utils::PrintTrace(fmt::format("Read setting {}='{}' from '{}'", key, item->second, _file.string()));
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
        throw UserException(
            fmt::format("Could not convert '{}' in '{}' to 'int'. ({})", value, _file.string(), e.what()));
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
            fmt::format("Delimiter property '{}' in '{}' must have length 1.", value, _file.string()));
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
    std::unordered_map<std::string, std::string> config;
    config["FormatName"] = "builtin CsvConfig";
    config["AccountOwner"] = "hokee";
    config["ColumnNames"] = "name;value";
    config["HasHeader"] = "false";
    config["IgnoreLines"] = "0";
    config["HasDoubleQuotes"] = "false";
    config["HasTrailingDelimiter"] = "false";
    config["Delimiter"] = "=";
    config["DateFormat"] = "dd.mm.yyyy";
    config["Category"] = "0";
    config["PayerPayee"] = "-1";
    config["Payer"] = "-1";
    config["Payee"] = "-1";
    config["Description"] = "1";
    config["Type"] = "-1";
    config["Date"] = "-1";
    config["Account"] = "-1";
    config["Value"] = "-1";
    CsvFormat format(config, "builtin CsvConfig format");
    return format;
}

} // namespace hokee