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
    SetExplorer("explorer");
    SetBrowser("start");
#elif __APPLE__
    SetExplorer("open");
    SetBrowser("open -a Safari");
#else
    SetExplorer("nautilus");
    SetBrowser("firefox");
#endif
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

const std::string Settings::GetExplorer() const
{
    return GetString("Explorer");
}

void Settings::SetExplorer(const std::string& value)
{
    SetString("Explorer", value);
}

const std::string Settings::GetBrowser() const
{
    return GetString("Browser");
}

void Settings::SetBrowser(const std::string& value)
{
    SetString("Browser", value);
}

} // namespace hokee