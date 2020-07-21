#pragma once

#include "Utils.h"
#include "csv/CsvDatabase.h"

#include <array>
#include <sstream>
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
    static constexpr const char* COPY_SAMPLES_CMD = "copy-samples";
    static constexpr const char* OPEN_CMD = "open";
    static constexpr const char* INPUT_CMD = "input";

    static std::string GetProgressPage(size_t value, size_t max);
    static std::string GetErrorPage(int errorCode, const std::string& errorMessage);
    static std::string GetSummaryPage(const CsvDatabase& pDatabase);
    static std::string GetItemPage(const CsvDatabase& pDatabase, int id);
    static std::string GetTablePage(const CsvDatabase& pDatabase, const std::string& title, const CsvTable& data);

    static std::string GetEmptyInputPage();
    static void GetItemReference(std::stringstream& output, int id);
    static void GetItemsReference(std::stringstream& output, int year, int month, const std::string& category,
                                  const std::string& text);

    static std::string GetButton(const std::string& link, const std::string& tooltip, const std::string& image,
                                 const std::string& text);
    static std::string GetEditorReference(const fs::path& file);
};

} // namespace hokee