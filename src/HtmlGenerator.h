#pragma once

#include "Utils.h"
#include "csv/CsvDatabase.h"

#include <array>

namespace hokee
{
class HtmlGenerator
{
    static std::string GetHead();
    static std::string GetHeader(const CsvDatabase& pDatabase);
    static std::string GetItemStart(const CsvDatabase& pDatabase, const std::string& title);
    static std::string GetItemEnd();
    static std::string GetTableStart();
    static std::string GetTableRow(CsvItem* row);
    static std::string GetTableEnd();

  public:
    HtmlGenerator() = delete;
    ~HtmlGenerator() = delete;

    HtmlGenerator(const HtmlGenerator&) = delete;
    HtmlGenerator& operator=(const HtmlGenerator&) = delete;
    HtmlGenerator(HtmlGenerator&&) = delete;
    HtmlGenerator& operator=(HtmlGenerator&&) = delete;

    static constexpr const char* INDEX_HTML = "index.html";
    static constexpr const char* ALL_HTML = "all.html";
    static constexpr const char* ASSIGNED_HTML = "assigned.html";
    static constexpr const char* UNASSIGNED_HTML = "unassigned.html";
    static constexpr const char* RULES_HTML = "rules.html";
    static constexpr const char* ISSUES_HTML = "issues.html";
    static constexpr const char* ITEM_HTML = "item.html";
    static constexpr const char* ITEMS_HTML = "items.html";
    static constexpr const char* SEARCH_HTML = "search.html";
    static constexpr const char* SETTINGS_CMD = "settings";
    static constexpr const char* EXIT_CMD = "exit";
    static constexpr const char* EDIT_CMD = "edit";
    static constexpr const char* RELOAD_CMD = "reload";

    static std::string GetProgressPage(size_t value, size_t max);
    static std::string GetErrorPage(int errorCode, const std::string& errorMessage);
    static std::string GetSummaryPage(const CsvDatabase& pDatabase);
    static std::string GetItemPage(const CsvDatabase& pDatabase, int id);
    static std::string GetTablePage(const CsvDatabase& pDatabase, const std::string& title, const CsvTable& data);
};

} // namespace hokee