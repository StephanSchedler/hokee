#include "csv/CsvDatabase.h"
#include "InternalException.h"
#include "Utils.h"
#include "csv/CsvDate.h"
#include "csv/CsvItem.h"
#include "csv/CsvParser.h"
#include "csv/CsvWriter.h"
#include "html/HtmlGenerator.h"

#include <fmt/core.h>
#include <fmt/format.h>

#include <chrono>
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
        if (rule1->Category.empty())
        {
            if (rule1->Issues.size() == 0)
            {
                Issues.push_back(rule1);
            }
            rule1->Issues.push_back("ERROR: Category must not be empty!");
        }

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
                rule1->Issues.push_back(fmt::format("ERROR: Redefinition of rule {}", rule2->Id));
            }
        }
    }
}

std::vector<std::string> CsvDatabase::GetCategories() const
{
    std::vector<std::string> categories;
    for (auto& rule : Rules)
    {
        if (std::find(categories.begin(), categories.end(), rule->Category) == categories.end())
        {
            categories.push_back(rule->Category);
        }
    }
    std::sort(categories.begin(), categories.end());
    return categories;
}

int CsvDatabase::DeleteRule(int id)
{
    Data.DeleteItem(id);
    Assigned.DeleteItem(id);
    Unassigned.DeleteItem(id);
    Issues.DeleteItem(id);
    return Rules.DeleteItem(id);
}

int CsvDatabase::NewRule(int itemId)
{
    std::shared_ptr<CsvItem> item = nullptr;
    for (auto& i : Data)
    {
        if (i->Id == itemId)
        {
            item = i;
            break;
        }
    }
    if (!item)
    {
        throw InternalException(__FILE__, __LINE__, fmt::format("Could not find item id {}", itemId));
    }

    CsvRowShared newRule = std::make_shared<CsvItem>(*item);

    newRule->Id = Utils::GenerateId();
    newRule->Date = {};
    newRule->Value = {};
    newRule->Account = {};
    newRule->Issues.clear();
    newRule->File = "???";
    newRule->Line = -1;
    newRule->References.clear();
    newRule->References.push_back(item.get());

    Rules.push_back(newRule);

    return newRule->Id;
}

void CsvDatabase::LoadRules(const fs::path& ruleSetFile)
{
    std::unique_ptr<CsvParser> csvReader;
    csvReader = std::make_unique<CsvParser>(ruleSetFile, CsvRules::GetFormat());
    csvReader->Load(Rules);
}

void CsvDatabase::MatchRules()
{
    Utils::PrintInfo("Match rules...");
    auto startTime = std::chrono::high_resolution_clock::now();

    // Clear rules
    for (auto& row : Data)
    {
        row->Issues.clear();
        row->References.clear();
    }
    Assigned.clear();
    Unassigned.clear();

    // Lower input
    for (auto& row : Data)
    {
        row->ToLower();
    }

    // Match rules
    for (uint64_t r = 0; r < Rules.size(); ++r)
    {
        auto& rule = Rules[r];
        rule->ToLower();
        rule->UpdateRegex();

        // reset
        rule->Issues.clear();
        rule->References.clear();

        // match
        for (auto& row : Data)
        {
            row->Match(rule);
        }
    }

    // Apply rules
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
    auto endTime = std::chrono::high_resolution_clock::now();
    Utils::PrintInfo(fmt::format(
        "Matched rules in {}ms",
        static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count())
            / 1000000));

    CheckRules();
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
    ProgressMax = 0;
    for (const auto& dir : fs::directory_iterator(inputDirectory))
    {
        if (fs::is_directory(dir))
        {
            if (dir.path().filename().string().rfind(".", 0) == 0)
            {
                // Utils::PrintInfo(fmt::format("Skip hidden directory '{}':", dir.path().string()));
                continue;
            }

            if (fs::is_empty(dir.path()))
            {
                // Utils::PrintInfo(fmt::format("Skip empty directory '{}':", dir.path().string()));
                continue;
            }

            for (const auto& file : fs::directory_iterator(dir.path()))
            {
                std::ignore = file;
                ProgressMax++;
            }
        }
    }

    ProgressValue = 0;
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
                ProgressValue++;
                if (!fs::is_regular_file(file) || Utils::ToLower(file.path().filename().string()) == "format.ini")
                {
                    continue;
                }
                Utils::PrintInfo(
                    fmt::format("Parse '{}' {}/{}...", file.path().string(), ProgressValue, ProgressMax));

                std::unique_ptr<CsvParser> csvReader;
                csvReader = std::make_unique<CsvParser>(file.path(), format);
                csvReader->Load(Data);
            }
        }
    }

    Sort(Data);
    LoadRules(ruleSetFile);
    MatchRules();
    MatchRules();
    MatchRules();
    Utils::PrintInfo("Finished loading.");
}
} // namespace hokee