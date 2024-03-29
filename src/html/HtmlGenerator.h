#pragma once

#include "HtmlElement.h"
#include "Utils.h"
#include "csv/CsvDatabase.h"

#include <array>
#include <sstream>
namespace hokee
{
class HtmlGenerator
{
    static HtmlElement* AddHtmlHead(HtmlElement* html);
    static HtmlElement* AddNavigationHeader(HtmlElement* body, const CsvDatabase& database);
    static void AddSummaryTableHeader(HtmlElement* table, int minYear, int maxYear);
    static void AddItemTableHeader(HtmlElement* table);
    static void AddItemTableRow(HtmlElement* table, CsvItem* row);

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
    static constexpr const char* HELP_HTML = "https://github.com/StephanSchedler/hokee/blob/master/README.md";
    static constexpr const char* BACKUP_HTML = "backup.html";
    static constexpr const char* SUPPORT_HTML = "support.html";
    static constexpr const char* EDIT_HTML = "edit.html";
    static constexpr const char* SETTINGS_HTML = "settings.html";
    static constexpr const char* EXIT_CMD = "exit.cmd";
    static constexpr const char* DELETE_CMD = "delete.cmd";
    static constexpr const char* NEW_CMD = "new.cmd";
    static constexpr const char* BACKUP_CMD = "backup.cmd";
    static constexpr const char* RESTORE_CMD = "restore.cmd";
    static constexpr const char* SAVE_CMD = "save.cmd";
    static constexpr const char* SAVE_RULE_CMD = "save-rule.cmd";
    static constexpr const char* RELOAD_CMD = "reload.cmd";
    static constexpr const char* COPY_SAMPLES_CMD = "copy-samples.cmd";
    static constexpr const char* OPEN_CMD = "open.cmd";
    static constexpr const char* INPUT_CMD = "input.cmd";

    static std::string GetBackupPage(const CsvDatabase& database, const fs::path& ruleSetFile);
    static std::string GetProgressPage(size_t value, size_t max);
    static std::string GetSupportPage(const CsvDatabase& database, const fs::path& ruleSetFile,
                                      const fs::path& inputDir);
    static std::string GetErrorPage(int errorCode, const std::string& errorMessage);
    static std::string GetSummaryPage(const CsvDatabase& database);
    static std::string GetItemPage(const CsvDatabase& database, int id, int flag);
    static std::string GetEditPage(const CsvDatabase& database, const fs::path& file, bool saved);
    static std::string GetSettingsPage(const CsvDatabase& database, const fs::path& file, bool saved);
    static std::string GetTablePage(const CsvDatabase& database, std::string title, const CsvTable& data, int filter);

    static std::string GetEmptyInputPage();

    static void AddInputForm(HtmlElement* table, const std::string& name, const std::string& value,
                             const std::string& description);
    static HtmlElement* AddButton(HtmlElement* tableRow, const std::string& link, const std::string& tooltip,
                          const std::string& image, const std::string& text, const std::string& style = "");
};

} // namespace hokee