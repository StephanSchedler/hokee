#pragma once

#include "csv/CsvFormat.h"

#include <string>
#include <string_view>

#if (defined(__clang_major__) && __clang_major__ < 9) || (defined(_MSVC_VER) && _MSVC_VER < 1914)                 \
    || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 8)
#include <experimental/filesystem>
#else
#include <filesystem>
#endif

#if (defined(__clang_major__) && __clang_major__ < 9) || (defined(_MSVC_VER) && _MSVC_VER < 1914)                 \
    || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 8)
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Utils
{
std::vector<std::string> SplitLine(const std::string& s, const CsvFormat& format);

const CsvFormat GetCsvFormat(const std::string& formatName);

void PrintInfo(std::string_view msg);
void PrintWarning(std::string_view msg);
void PrintError(std::string_view msg);

std::string Run(const char* cmd);

bool AskYesNoQuestion(const std::string& question, bool defaultYes = true, bool batchMode = false);
int GetUniqueId();
bool ExtractMissingString(std::string& extracted, const std::string& original, const std::string& missing);


std::string GetEnv(const std::string& name);
fs::path GetHomePath();

} // namespace Utils