#include "Settings.h"
#include "InternalException.h"

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
SetServerPort("12345");
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

int Settings::GetServerPort() const
{
    std::string portString = GetString("Port", "0");
    int port = 0;
    try {
        port = std::stoi(portString);
    }
    catch (std::exception& e) 
    {
        throw UserException(fmt::format("Could not convert Port config string '{}' to int. ({})", portString, e.what()));
    }
    return port;
}

void Settings::SetBrowser(const std::string& value)
{
    SetString("Browser", value);
}

void Settings::SetServerPort(const std::string& value)
{
    SetString("Port", value);
}

} // namespace hokee