#include "CsvRules.h"

namespace hokee
{
    CsvFormat CsvRules::GetFormat()
    {
        CsvFormat format;
        format.SetColumnNames({"Category", "Payer/Payee", "Description", "Type", "Date", "Account", "Value"});
        format.SetHasHeader(true);
        format.SetIgnoreLines(4);
        format.SetHasDoubleQuotes(false);
        format.SetHasTrailingDelimiter(false);
        format.SetDelimiter(';');
        format.SetDateFormat("dd.mm.yyyy");
        format.SetCategory(0);
        format.SetPayerPayee(1);
        format.SetPayer(-1);
        format.SetPayee(-1);
        format.SetDescription(2);
        format.SetType(3);
        format.SetDate(4);
        format.SetAccount(5);
        format.SetValue(6);
        return format;
    }

    std::vector<std::string> CsvRules::GetCategories()
    {
        auto rulesFormat = CsvRules::GetFormat();
        const auto rulesHeader = this->GetCsvHeader();
        if (rulesHeader.size() != rulesFormat.GetIgnoreLines())
        {
            throw InternalException(__FILE__, __LINE__, "Internal header of rule set does not match expected format!");
        }
        
        return Utils::SplitLine(rulesHeader[1], rulesFormat);
    }
} // namespace hokee