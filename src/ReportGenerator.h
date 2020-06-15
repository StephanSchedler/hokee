#pragma once

#include "CsvDatabase.h"
#include "Utils.h"

#include <array>

namespace hokeeboo
{

class ReportGenerator
{
    CsvDatabase* _csvDatabase;
    fs::path _outputDirectory;
    const fs::path _itemsDirectory{"items"};
    const fs::path _indexHtml{"index.html"};
    const fs::path _allHtml{"all.html"};
    const fs::path _assignedHtml{"assigned.html"};
    const fs::path _unassignedHtml{"unassigned.html"};
    const fs::path _rulesHtml{"rules.html"};
    const fs::path _issuesHtml{"issues.html"};

    std::string GetHeadline(const std::string& urlPrefix);
    std::string GetPageStart(const std::string& title, const std::string& urlPrefix = "");
    std::string GetPageEnd();
    std::string GetTableStart();
    std::string GetTableRow(CsvItem* row, const std::string& urlPrefix = "");
    std::string GetTableEnd();
    std::string GetIndexPage();
    std::string GetTablePage(const std::string& title, const CsvTable& data);
    std::string GetItemPage(const std::string& title, const std::shared_ptr<CsvItem>& row);

  public:
    ReportGenerator(CsvDatabase* database);
    ~ReportGenerator() = default;

    ReportGenerator(const ReportGenerator&) = delete;
    ReportGenerator& operator=(const ReportGenerator&) = delete;
    ReportGenerator(ReportGenerator&&) = delete;
    ReportGenerator& operator=(ReportGenerator&&) = delete;

    void Write(const fs::path& outputDirectory);
    void PrintIssues();
};

} // namespace hokeeboo