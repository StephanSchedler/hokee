#include "Settings.h"

#include <fmt/format.h>

namespace hokee
{
Settings::Settings()
    : CsvConfig()
{
    SetInputDirectory("input");
    SetRuleSetFile("rules.csv");
#ifdef _MSC_VER
    SetEditor("notepad");
    SetBrowser("start");
#elif __APPLE__
    SetEditor("open -a TextEdit");
    SetBrowser("open -a Safari");
#else
    SetEditor("nano");
    SetBrowser("firefox");
#endif
    SetAddRules("true");
    SetUpdateRules("true");
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

const std::string Settings::GetBrowser() const
{
    return GetString("Browser");
}

void Settings::SetBrowser(const std::string& value)
{
    SetString("Browser", value);
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

} // namespace hokee