#include "CsvDatabase.h"
#include "CsvDate.h"
#include "CsvItem.h"
#include "CsvParser.h"
#include "CsvWriter.h"
#include "CustomException.h"
#include "ReportGenerator.h"
#include "Utils.h"

#include <fmt/format.h>

#include <functional>
#include <memory>
#include <regex>

namespace hokeeboo
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

void CsvDatabase::FixRules(const fs::path& rulesCsv)
{
    ReportGenerator reportGenerator(this);
    reportGenerator.PrintIssues();

    if (std::system(fmt::format("code --wait {}", fs::absolute(rulesCsv).string()).c_str()) < 0)
    {
        throw CustomException(__FILE__, __LINE__, "Could not open rule editor.");
    }
}

void CsvDatabase::AddRules(const fs::path& rulesCsv, const fs::path& workingDirectory)
{
    std::vector<std::string> categories;
    for (auto& rule : Rules)
    {
        if (std::find(categories.begin(), categories.end(), rule->Category) == categories.end())
        {
            categories.push_back(rule->Category);
        }
    }
    fs::path unassignedCsv = workingDirectory / "unassigned.csv";
    Utils::PrintInfo(fmt::format("  Open {}", unassignedCsv.string()));
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
    CsvWriter::Write(unassignedCsv, Unassigned);

    Utils::PrintInfo(fmt::format("Open {}", unassignedCsv.string()));

    if (std::system(fmt::format("code --wait {}", fs::absolute(unassignedCsv).string()).c_str()) < 0)
    {
        throw CustomException(__FILE__, __LINE__, "Could not open rule editor.");
    }

    Utils::PrintInfo(fmt::format("Reload {}", unassignedCsv.string()));
    CsvParser csvParser(unassignedCsv, Utils::GetCsvFormat("Rules"));
    CsvTable importedTable;
    csvParser.Load(importedTable);
    if (Unassigned.size() != importedTable.size())
    {
        throw CustomException(__FILE__, __LINE__, "(Re-)Imported table has different line count.");
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
            newRule->Date = unassigned->Date;
        }
        if (unassigned->Value != imported->Value)
        {
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
    CsvWriter::Write(rulesCsv, Rules);
}

void CsvDatabase::LoadRules(const fs::path& rulesCsv)
{
    std::unique_ptr<CsvParser> csvReader;
    Utils::PrintInfo(fmt::format("Parse {}...", rulesCsv.string()));

    csvReader = std::make_unique<CsvParser>(rulesCsv, Utils::GetCsvFormat("Rules"));
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

CsvDatabase::CsvDatabase(const fs::path& inputDirectory, const fs::path& rulesCsv)
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
            for (const auto& file : fs::directory_iterator(dir.path()))
            {
                if (!fs::is_regular_file(file))
                {
                    continue;
                }
                Utils::PrintInfo(
                    fmt::format("    Parse '{}' {}/{}...", file.path().string(), ++fileCounter, numberOfFiles));

                std::unique_ptr<CsvParser> csvReader;
                std::string format = dir.path().filename().string();
                if (format == "DKB")
                {
                    csvReader = std::make_unique<CsvParser>(file.path(), Utils::GetCsvFormat("DKB"), "VISA");
                }
                else if (format == "OSPA_Stephan")
                {
                    csvReader = std::make_unique<CsvParser>(file.path(), Utils::GetCsvFormat("OSPA"), "Stephan");
                }
                else if (format == "OSPA2_Stephan")
                {
                    csvReader = std::make_unique<CsvParser>(file.path(), Utils::GetCsvFormat("OSPA2"), "Stephan");
                }
                else if (format == "OSPA_Sandra")
                {
                    csvReader = std::make_unique<CsvParser>(file.path(), Utils::GetCsvFormat("OSPA"), "Sandra");
                }
                else if (format == "OSPA2_Sandra")
                {
                    csvReader = std::make_unique<CsvParser>(file.path(), Utils::GetCsvFormat("OSPA2"), "Sandra");
                }
                else if (format == "Postbank")
                {
                    csvReader = std::make_unique<CsvParser>(file.path(), Utils::GetCsvFormat("Postbank"),
                                                            "Sandra Wagner");
                }
                else if (format == "ABC")
                {
                    csvReader = std::make_unique<CsvParser>(file.path(), Utils::GetCsvFormat("ABC"), "Mr. X");
                }
                else
                {
                    throw CustomException(__FILE__, __LINE__,
                                          fmt::format("Unknown input format: {}", format).c_str());
                }

                csvReader->Load(Data);
            }
        }
    }

    Sort(Data);

    if (!fs::exists(rulesCsv))
    {
        Utils::PrintWarning(fmt::format("Could not find rules. Create empty rules file {}", rulesCsv.string()));
        CsvTable empty;
        empty.Header.push_back("Categories:");
        empty.Header.push_back("Categorie1;Categorie2;!Ignore");
        empty.Header.push_back("");
        empty.Header.push_back("Rules:");
        CsvWriter::Write(rulesCsv, empty);
    }
    LoadRules(rulesCsv);
    MatchRules();
    CheckRules();
}
} // namespace hokeeboo