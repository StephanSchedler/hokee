#pragma once

#include "csv/CsvParser.h"
#include "csv/CsvRules.h"
#include "Utils.h"

#include <array>
#include <atomic>

namespace hokee
{

class CsvDatabase
{
    void LoadRules(const fs::path& ruleSetFile);
    void MatchRules();
    void CheckRules();
    void Sort(CsvTable& csvData);

  public:
    CsvTable Data{};
    CsvTable Unassigned{};
    CsvTable Assigned{};

    CsvRules Rules{};
    CsvTable Issues{};

    CsvDatabase() = default;
    ~CsvDatabase() = default;

    CsvDatabase(const CsvDatabase&) = delete;
    CsvDatabase& operator=(const CsvDatabase&) = delete;
    CsvDatabase(CsvDatabase&&) = delete;
    CsvDatabase& operator=(CsvDatabase&&) = delete;

    void Load(const fs::path& inputDirectory, const fs::path& ruleSetFile);
    void UpdateRules(const fs::path& ruleSetFile, const std::string& editor);
    void AddRules(const fs::path& ruleSetFile, const fs::path& workingDirectory, const std::string& editor);
    
    std::atomic<size_t> ProgressMax{100};
    std::atomic<size_t> ProgressValue{0};
};

} // namespace hokee