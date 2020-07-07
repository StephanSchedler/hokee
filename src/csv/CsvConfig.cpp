#include "csv/CsvConfig.h"
#include "UserException.h"
#include "csv/CsvParser.h"
#include "csv/CsvTable.h"

#include <fmt/format.h>

namespace hokee
{
CsvConfig::CsvConfig()
{
    _config["InputDirectory"] = "input/";
    _config["OutputDirectory"] = "output/";
    _config["RuleSetFile"] = "rules.csv";
#ifdef _MSC_VER
    _config["Editor"] = "notepad";
#else
    _config["Editor"] = "nano";
#endif
    _config["AddRules"] = "true";
    _config["UpdateRules"] = "true";
    _config["GenerateReport"] = "true";
}

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

fs::path CsvConfig::GetInputDirectory()
{
    return GetString("InputDirectory");
}

void CsvConfig::SetInputDirectory(const fs::path& value)
{
    SetString("InputDirectory", value.string());
}

fs::path CsvConfig::GetOutputDirectory()
{
    return GetString("OutputDirectory");
}

void CsvConfig::SetOutputDirectory(const fs::path& value)
{
    SetString("OutputDirectory", value.string());
}

fs::path CsvConfig::GetRuleSetFile()
{
    return GetString("RuleSetFile");
}

void CsvConfig::SetRuleSetFile(const fs::path& value)
{
    SetString("RuleSetFile", value.string());
}

std::string CsvConfig::GetEditor()
{
    return GetString("Editor");
}

void CsvConfig::SetEditor(const std::string& value)
{
    SetString("Editor", value);
}

bool CsvConfig::GetAddRules()
{
    return GetBool("AddRules");
}

void CsvConfig::SetAddRules(bool value)
{
    SetString("AddRules", value ? "true" : "false");
}

bool CsvConfig::GetUpdateRules()
{
    return GetBool("UpdateRules");
}

void CsvConfig::SetUpdateRules(bool value)
{
    SetString("UpdateRules", value ? "true" : "false");
}

bool CsvConfig::GetGenerateReport()
{
    return GetBool("GenerateReport");
}

void CsvConfig::SetGenerateReport(bool value)
{
    SetString("GenerateReport", value ? "true" : "false");
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

    // std::string
    output << "InputDirectory=" << GetString("InputDirectory") << std::endl;
    output << "OutputDirectory=" << GetString("OutputDirectory") << std::endl;
    output << "RuleSetFile=" << GetString("RuleSetFile") << std::endl;
    output << "Editor=" << GetString("Editor") << std::endl;

    // boolean
    output << "AddRules=" << (GetBool("AddRules") ? "true" : "false") << std::endl;
    output << "UpdateRules=" << (GetBool("UpdateRules") ? "true" : "false") << std::endl;
    output << "GenerateReport=" << (GetBool("GenerateReport") ? "true" : "false") << std::endl;
}

} // namespace hokee