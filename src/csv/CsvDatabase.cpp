#include "csv/CsvDatabase.h"
#include "HtmlGenerator.h"
#include "InternalException.h"
#include "Utils.h"
#include "csv/CsvDate.h"
#include "csv/CsvItem.h"
#include "csv/CsvParser.h"
#include "csv/CsvWriter.h"

#include <fmt/format.h>

#include <functional>
#include <memory>
#include <regex>
#include <sstream>

namespace hokee
{
void CsvDatabase::Sort(CsvTable& csvData)
{
    auto compareDates = [](const CsvRowShared& i, const CsvRowShared& j) -> bool { return (i->Date < j->Date); };
    std::sort(csvData.begin(), csvData.end(), compareDates);
}

void CsvDatabase::CheckRules()
{
    Utils::PrintInfo("Check rules...");
    Issues.clear();

    for (auto& row : Data)
    {
        if (row->References.size() > 1)
        {
            for (auto& ref : row->References)
            {
                if (ref->Category != row->References[0]->Category)
                {
                    row->Issues.push_back("ERROR: Multiple rules with "
                                          "different categories are matching");
                    Issues.push_back(row);
                    break;
                }
            }
        }
    }

    for (auto& rule1 : Rules)
    {
        if (rule1->References.size() == 0)
        {
            if (rule1->Issues.size() == 0)
            {
                Issues.push_back(rule1);
            }
            rule1->Issues.push_back("ERROR: Rule does not match any item!");
        }

        bool isAlreadyCovered = true;
        for (auto& ref : rule1->References)
        {
            isAlreadyCovered = isAlreadyCovered && ref->References.size() > 1;
        }
        if (isAlreadyCovered)
        {
            if (rule1->Issues.size() == 0)
            {
                Issues.push_back(rule1);
            }
            rule1->Issues.push_back("ERROR: Rule is redundant. (Matches are covered by other rules)!");
        }

        for (auto& rule2 : Rules)
        {
            if (rule1.get() != rule2.get() && *rule1 == *rule2)
            {
                if (rule1->Issues.size() == 0)
                {
                    Issues.push_back(rule1);
                }
                std::stringstream issueStream{};
                issueStream << "ERROR: Redefinition of rule ";
                HtmlGenerator::GetItemReference(issueStream, rule2->Id);
                issueStream << "(";
                issueStream << HtmlGenerator::GetEditorReference(rule2->File);
                issueStream << ")!";
                rule1->Issues.push_back(issueStream.str());
            }
        }
    }
}

void CsvDatabase::AddRules(const fs::path& ruleSetFile, const std::string& editor)
{
    std::vector<std::string> categories = Rules.GetCategories();
    Utils::PrintInfo("Supported categories:");
    std::string categoryString;
    int categoryCounter = 0;
    for (const auto& cat : categories)
    {
        std::cout << "\t" << cat << " (" << cat.substr(0, 3) << ")";
        if (++categoryCounter % 6 == 0)
        {
            std::cout << std::endl;
        }
        categoryString += cat.substr(0, 3) + ' ';
    }
    std::cout << std::endl;
    for (auto& row : Unassigned)
    {
        row->Category = categoryString;
    }

    std::vector<std::string> header{};
    header.push_back(
        "# The table below lists all items that do not match a rule. If you delete strings in a cell, these are ");
    header.push_back(
        "# used to create a new rule. If you delete strings in multiple cells, all strings need to match. Do not");
    header.push_back("# delete or reorder rows. If you do not want to add a rule, leave the row unmodified.");
    header.push_back("");
    Unassigned.SetCsvHeader(std::move(header));

    fs::path unassignedCsv = Utils::GetTempDir() / "unassigned.csv";
    Utils::PrintInfo(fmt::format("Open {}", unassignedCsv.string()));
    CsvWriter::Write(unassignedCsv, Unassigned);

    Utils::PrintInfo(fmt::format("Open {}", unassignedCsv.string()));

    std::string cmd = fmt::format("{} \"{}\"", editor, fs::absolute(unassignedCsv).string());
    if (Utils::RunSync(cmd.c_str()) < 0)
    {
        throw UserException(fmt::format("Could not open editor: {}", cmd));
    }

    Utils::PrintInfo(fmt::format("Reload {}", unassignedCsv.string()));
    CsvParser csvParser(unassignedCsv, CsvRules::GetFormat());
    CsvTable importedTable;
    csvParser.Load(importedTable);
    if (Unassigned.size() != importedTable.size())
    {
        throw UserException(fmt::format(
            "Imported item count {} does not match expected value {}. You must not delete, add or reorder rows!",
            importedTable.size(), Unassigned.size()));
    }

    for (size_t i = 0; i < Unassigned.size(); ++i)
    {
        auto unassigned = Unassigned[i].get();
        auto imported = importedTable[i].get();
        auto newRule = std::make_shared<CsvItem>();

        bool isValid
            = Utils::ExtractMissingString(newRule->PayerPayee, unassigned->PayerPayee, imported->PayerPayee);
        isValid
            |= Utils::ExtractMissingString(newRule->Description, unassigned->Description, imported->Description);
        isValid |= Utils::ExtractMissingString(newRule->Type, unassigned->Type, imported->Type);

        if (unassigned->Date != imported->Date)
        {
            isValid = true;
            newRule->Date = unassigned->Date;
        }
        if (unassigned->Value != imported->Value)
        {
            isValid = true;
            newRule->Value = unassigned->Value;
        }

        isValid &= Utils::ExtractMissingString(newRule->Category, categoryString, imported->Category);
        if (isValid)
        {
            for (auto& cat : categories)
            {
                if (cat.rfind(newRule->Category, 0) == 0)
                {
                    newRule->Category = cat;
                    newRule->Id = Utils::GenerateId();
                    Rules.push_back(newRule);
                    break;
                }
            }
        }
    }
    CsvWriter::Write(ruleSetFile, Rules);
}

void CsvDatabase::LoadRules(const fs::path& ruleSetFile)
{
    std::unique_ptr<CsvParser> csvReader;
    csvReader = std::make_unique<CsvParser>(ruleSetFile, CsvRules::GetFormat());
    csvReader->Load(Rules);
}

void CsvDatabase::MatchRules()
{
    // Reset Results
    for (auto& row : Data)
    {
        row->Issues.clear();
        row->References.clear();
    }
    Assigned.clear();
    Unassigned.clear();

    // Match rules
    for (auto& rule : Rules)
    {
        rule->Issues.clear();
        rule->References.clear();
        for (auto& row : Data)
        {
            row->Match(rule);
        }
    }

    Unassigned.clear();
    for (auto& row : Data)
    {
        if (row->References.size() == 0)
        {
            Unassigned.push_back(row);
        }
        else
        {
            Assigned.push_back(row);
        }
    }
}

void CsvDatabase::Load(const fs::path& inputDirectory, const fs::path& ruleSetFile)
{
    // Clear
    Data.clear();
    Unassigned.clear();
    Assigned.clear();
    Rules.clear();
    Issues.clear();

    // detect number of file
    ProgressValue = 0;
    ProgressMax
        = std::distance(fs::recursive_directory_iterator(inputDirectory), fs::recursive_directory_iterator{});
    ProgressMax -= std::distance(fs::directory_iterator(inputDirectory), fs::directory_iterator{});

    for (const auto& dir : fs::directory_iterator(inputDirectory))
    {
        if (fs::is_directory(dir))
        {
            if (dir.path().filename().string().rfind(".", 0) == 0)
            {
                Utils::PrintInfo(fmt::format("Skip hidden directory '{}':", dir.path().string()));
                continue;
            }

            if (fs::is_empty(dir.path()))
            {
                Utils::PrintInfo(fmt::format("Skip empty directory '{}':", dir.path().string()));
                continue;
            }

            fs::path formatFile = dir.path() / "format.ini";
            if (!fs::exists(formatFile))
            {
                throw UserException(fmt::format("Could not find format description file"), formatFile);
            }
            CsvFormat format(formatFile);

            for (const auto& file : fs::directory_iterator(dir.path()))
            {
                if (!fs::is_regular_file(file) || Utils::ToLower(file.path().filename().string()) == "format.ini")
                {
                    continue;
                }
                Utils::PrintInfo(
                    fmt::format("Parse '{}' {}/{}...", file.path().string(), ++ProgressValue, ProgressMax));

                std::unique_ptr<CsvParser> csvReader;
                csvReader = std::make_unique<CsvParser>(file.path(), format);
                csvReader->Load(Data);
            }
        }
    }

    Sort(Data);
    LoadRules(ruleSetFile);
    MatchRules();
    CheckRules();
    Utils::PrintInfo("Done.");
}
} // namespace hokee