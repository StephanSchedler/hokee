#pragma once

#include "CsvFormat.h"

#include <string>
#include <string_view>


namespace Utils
{
    
const CsvFormat GetCsvFormat(const std::string& formatName);

void PrintInfo(std::string_view msg);
void PrintWarning(std::string_view msg);
void PrintError(std::string_view msg);

std::string Run(const char* cmd);

char AskYesNoQuestion(const std::string& question, bool defaultYes = true);
int GetUniqueId();
bool ExtractMissingString(std::string& extracted, const std::string& original, const std::string& missing);

} // namespace Utils