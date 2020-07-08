#pragma once

#include "csv/CsvFormat.h"

#include <string>
#include <string_view>

namespace hokee::Utils
{
std::vector<std::string> SplitLine(const std::string& s, char delimiter, bool hasTrailingDelimiter = false);
std::string ToLower(const std::string& str);
std::string ToUpper(const std::string& str);

void PrintInfo(std::string_view msg);
void PrintWarning(std::string_view msg);
void PrintError(std::string_view msg);

std::string Run(const char* cmd);

bool AskYesNoQuestion(const std::string& question, bool defaultYes = true, bool batchMode = false);
int GetUniqueId();
bool ExtractMissingString(std::string& extracted, const std::string& original, const std::string& missing);

std::string GetEnv(const std::string& name);
fs::path GetHomePath();

} // namespace hokee::Utils