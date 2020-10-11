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
void SetVerbose(bool verbose);
void PrintTrace(std::string_view msg);
void PrintInfo(std::string_view msg);
void PrintWarning(std::string_view msg);
void PrintError(std::string_view msg);

void RunSync(const std::string& cmd, const std::vector<std::string>& args = {});
void RunAsync(const std::string& cmd, const std::vector<std::string>& args = {});

bool AskYesNoQuestion(const std::string& question);
int GenerateId();
void ResetIdGenerator();
bool ExtractMissingString(std::string& extracted, const std::string& original, const std::string& missing);

std::string ReadFileContent(const fs::path& file);
void WriteFileContent(const fs::path& file, const std::string& content);
bool CompareFiles(const fs::path& file1, const fs::path& file2);

std::string GetEnv(const std::string& name);
fs::path GetHomeDir();
fs::path GetTempDir();

std::string EscapeHtml(std::string text);
std::string GenerateSupportMail(const fs::path& ruleSetFile, const fs::path& inputDir);
std::string GenerateTimestamp();

void TerminationHandler();
void TerminationHandler(const std::exception& e);
void TerminationHandler(const UserException& e);

} // namespace hokee::Utils