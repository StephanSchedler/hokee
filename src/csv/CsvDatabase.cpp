#include "csv/CsvDatabase.h"
#include "csv/CsvDate.h"
#include "csv/CsvItem.h"
#include "csv/CsvParser.h"
#include "csv/CsvWriter.h"
#include "InternalException.h"
#include "ReportGenerator.h"
#include "Utils.h"

#include <fmt/format.h>

#include <functional>
#include <memory>
#include <regex>

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
                rule1->Issues.push_back(
                    fmt::format("ERROR: Redefinition of rule <a "
                                "href=\"../items/{0}.html\">{0}</a> (<a href=\"../../{1}\">{2}</a> : {3})!",
                                rule2->Id, rule2->File.string(), rule2->File.filename().string(), rule2->Line));
            }
        }
    }
}

void CsvDatabase::UpdateRules(const fs::path& ruleSetFile, const std::string& editor)
{
    ReportGenerator reportGenerator(this);
    reportGenerator.PrintIssues();

    std::string cmd = fmt::format("{} \"{}\"", editor, fs::absolute(ruleSetFile).string());
    Utils::PrintInfo(fmt::format("Run: {}", cmd));
    if (std::system(cmd.c_str()) < 0)
    {
        throw UserException(fmt::format("Could not open editor: {}", cmd));
    }
}

void CsvDatabase::AddRules(const fs::path& ruleSetFile, const fs::path& workingDirectory, const std::string& editor)
{
    std::vector<std::string> categories = Rules.GetCategories();
    Utils::PrintInfo("  Supported categories:");
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
    header.push_back("# The table below lists all items that do not match a rule. If you delete strings in a cell, these are ");
    header.push_back("# used to create a new rule. If you delete strings in multiple cells, all strings need to match. Do not");
    header.push_back("# delete or reorder rows. If you do not want to add a rule, leave the row unmodified.");
    header.push_back("");
    Unassigned.SetCsvHeader(std::move(header));

    fs::path unassignedCsv = workingDirectory / "unassigned.csv";
    Utils::PrintInfo(fmt::format("  Open {}", unassignedCsv.string()));
    CsvWriter::Write(unassignedCsv, Unassigned);

    Utils::PrintInfo(fmt::format("Open {}", unassignedCsv.string()));

    std::string cmd = fmt::format("{} \"{}\"", editor, fs::absolute(unassignedCsv).string());
    Utils::PrintInfo(fmt::format("Run: {}", cmd));
    if (std::system(cmd.c_str()) < 0)
    {
        throw UserException(fmt::format("Could not open editor: {}", cmd));
    }

    Utils::PrintInfo(fmt::format("Reload {}", unassignedCsv.string()));
    CsvParser csvParser(unassignedCsv, CsvRules::GetFormat());
    CsvTable importedTable;
    csvParser.Load(importedTable);
    if (Unassigned.size() != importedTable.size())
    {
        throw UserException(fmt::format("Imported item count {} does not match expected value {}. You must not delete, add or reorder rows!", importedTable.size(), Unassigned.size()));
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
                    newRule->Id = Utils::GetUniqueId();
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
    Utils::PrintInfo(fmt::format("Parse {}...", ruleSetFile.string()));

    csvReader = std::make_unique<CsvParser>(ruleSetFile, CsvRules::GetFormat());
    csvReader->Load(Rules);
}

void CsvDatabase::MatchRules()
{
    Utils::PrintInfo("Match rules...");
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

CsvDatabase::CsvDatabase(const fs::path& inputDirectory, const fs::path& ruleSetFile)
{
    // detect number of file
    int fileCounter = 0;
    auto numberOfFiles
        = std::distance(fs::recursive_directory_iterator(inputDirectory), fs::recursive_directory_iterator{});
    numberOfFiles -= std::distance(fs::directory_iterator(inputDirectory), fs::directory_iterator{});

    for (const auto& dir : fs::directory_iterator(inputDirectory))
    {
        if (fs::is_directory(dir))
        {
            if (dir.path().filename().string().rfind(".", 0) == 0)
            {
                Utils::PrintInfo(fmt::format("  Skip directory '{}':", dir.path().string()));
                continue;
            }

            Utils::PrintInfo(fmt::format("  Found directory '{}':", dir.path().string()));
            fs::path formatFile = dir.path() / "format.ini";
            if (!fs::exists(formatFile))
            {
                throw UserException(fmt::format("Could not find format description: {}", formatFile.string()));
            }
            CsvFormat format(formatFile);

            for (const auto& file : fs::directory_iterator(dir.path()))
            {
                if (!fs::is_regular_file(file))
                {
                    continue;
                }
                Utils::PrintInfo(
                    fmt::format("    Parse '{}' {}/{}...", file.path().string(), ++fileCounter, numberOfFiles));

                std::unique_ptr<CsvParser> csvReader;
                csvReader = std::make_unique<CsvParser>(file.path(), format);
                csvReader->Load(Data);
            }
        }
    }

    Sort(Data);

    if (!fs::exists(ruleSetFile))
    {
        Utils::PrintWarning(fmt::format("Could not find rules. Create empty rules file {}", ruleSetFile.string()));
        std::vector<std::string> header{};
        header.push_back("Categories:");
        header.push_back("Categorie1;Categorie2;Ignore!");
        header.push_back("");
        header.push_back("Rules:");
        
        CsvTable empty;
        empty.SetCsvHeader(std::move(header));
        CsvWriter::Write(ruleSetFile, empty);
    }
    LoadRules(ruleSetFile);
    MatchRules();
    CheckRules();
}
} // namespace hokee