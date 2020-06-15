#pragma once

#include "CsvParser.h"
#include "CsvTable.h"
#include "Utils.h"

#include <array>

namespace hokeeboo
{

class CsvDatabase
{
    void LoadRules(const fs::path& rulesCsv);
    void MatchRules();
    void CheckRules();
    void Sort(CsvTable& csvData);

  public:
    CsvTable Data;
    CsvTable Unassigned;
    CsvTable Assigned;

    CsvTable Rules;
    CsvTable Issues;

    CsvDatabase(const fs::path& inputDirectory, const fs::path& rulesCsv);
    ~CsvDatabase() = default;

    CsvDatabase(const CsvDatabase&) = delete;
    CsvDatabase& operator=(const CsvDatabase&) = delete;
    CsvDatabase(CsvDatabase&&) = delete;
    CsvDatabase& operator=(CsvDatabase&&) = delete;

    void FixRules(const fs::path& rulesCsv);
    void AddRules(const fs::path& rulesCsv, const fs::path& workingDirectory);
};

} // namespace hokeeboo