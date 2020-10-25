#pragma once

#include "csv/CsvParser.h"
#include "csv/CsvRules.h"
#include "Utils.h"

#include <array>
#include <atomic>
#include <vector>

namespace hokee
{

class CsvDatabase
{
    void LoadRules(const fs::path& ruleSetFile);
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
    void MatchRules();
    int NewRule(int id);
    int DeleteRule(int id);
    std::vector<std::string> GetCategories() const;
    
    std::atomic<size_t> ProgressMax{100};
    std::atomic<size_t> ProgressValue{0};
};

} // namespace hokee