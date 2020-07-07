#include "Settings.h"

#include <fmt/format.h>

namespace hokee
{
Settings::Settings()
    : CsvConfig()
{
    SetInputDirectory("input");
    SetOutputDirectory("output");
    SetRuleSetFile("rules.csv");
#ifdef _MSC_VER
    SetEditor("notepad");
#else
    SetEditor("nano");
#endif
    SetAddRules("true");
    SetUpdateRules("true");
    SetGenerateReport("true");
}

Settings::Settings(fs::path file)
    : CsvConfig(file)
{
}

fs::path Settings::GetInputDirectory()
{
    return GetString("InputDirectory");
}

void Settings::SetInputDirectory(const fs::path& value)
{
    SetString("InputDirectory", value.string());
}

fs::path Settings::GetOutputDirectory()
{
    return GetString("OutputDirectory");
}

void Settings::SetOutputDirectory(const fs::path& value)
{
    SetString("OutputDirectory", value.string());
}

fs::path Settings::GetRuleSetFile()
{
    return GetString("RuleSetFile");
}

void Settings::SetRuleSetFile(const fs::path& value)
{
    SetString("RuleSetFile", value.string());
}

std::string Settings::GetEditor()
{
    return GetString("Editor");
}

void Settings::SetEditor(const std::string& value)
{
    SetString("Editor", value);
}

bool Settings::GetAddRules()
{
    return GetBool("AddRules");
}

void Settings::SetAddRules(bool value)
{
    SetString("AddRules", value ? "true" : "false");
}

bool Settings::GetUpdateRules()
{
    return GetBool("UpdateRules");
}

void Settings::SetUpdateRules(bool value)
{
    SetString("UpdateRules", value ? "true" : "false");
}

bool Settings::GetGenerateReport()
{
    return GetBool("GenerateReport");
}

void Settings::SetGenerateReport(bool value)
{
    SetString("GenerateReport", value ? "true" : "false");
}

} // namespace hokee