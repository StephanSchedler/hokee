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

Settings::Settings(const fs::path& file)
    : CsvConfig(file)
{
}

const fs::path Settings::GetInputDirectory() const
{
    return GetString("InputDirectory");
}

void Settings::SetInputDirectory(const fs::path& value)
{
    SetString("InputDirectory", value.string());
}

const fs::path Settings::GetOutputDirectory() const
{
    return GetString("OutputDirectory");
}

void Settings::SetOutputDirectory(const fs::path& value)
{
    SetString("OutputDirectory", value.string());
}

const fs::path Settings::GetRuleSetFile() const
{
    return GetString("RuleSetFile");
}

void Settings::SetRuleSetFile(const fs::path& value)
{
    SetString("RuleSetFile", value.string());
}

const std::string Settings::GetEditor() const
{
    return GetString("Editor");
}

void Settings::SetEditor(const std::string& value)
{
    SetString("Editor", value);
}

bool Settings::GetAddRules() const
{
    return GetBool("AddRules");
}

void Settings::SetAddRules(bool value)
{
    SetBool("AddRules", value);
}

bool Settings::GetUpdateRules() const
{
    return GetBool("UpdateRules");
}

void Settings::SetUpdateRules(bool value)
{
    SetBool("UpdateRules", value);
}

bool Settings::GetGenerateReport() const
{
    return GetBool("GenerateReport");
}

void Settings::SetGenerateReport(bool value)
{
    SetBool("GenerateReport", value);
}

} // namespace hokee