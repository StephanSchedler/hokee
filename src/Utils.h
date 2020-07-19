#pragma once

#include "csv/CsvFormat.h"
#include "UserException.h"

#include <string>
#include <vector>
#include <string_view>

namespace hokee::Utils
{
std::vector<std::string> SplitLine(const std::string& s, char delimiter, bool hasTrailingDelimiter = false);
std::string ToLower(const std::string& str);
std::string ToUpper(const std::string& str);

const std::vector<std::string> GetLastMessages();
void PrintTrace(std::string_view msg);
void PrintInfo(std::string_view msg);
void PrintWarning(std::string_view msg);
void PrintError(std::string_view msg);

std::string Run(const char* cmd);

bool AskYesNoQuestion(const std::string& question, bool defaultYes = true, bool batchMode = false);
int GenerateId();
void EditFile(const fs::path& file, const std::string& editor);
void OpenFolder(const fs::path& folder, const std::string& explorer);
void ResetIdGenerator();
bool ExtractMissingString(std::string& extracted, const std::string& original, const std::string& missing);

std::string GetEnv(const std::string& name);
fs::path GetHomePath();


void TerminationHandler(bool pause);
void TerminationHandler(const std::exception& e, bool pause);
void TerminationHandler(const UserException& e, bool pause);

} // namespace hokee::Utils