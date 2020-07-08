#include "CsvRules.h"

namespace hokee
{
const CsvFormat CsvRules::GetFormat()
{
    std::unordered_map<std::string, std::string> config;
    config["FormatName"] = "builtin CsvRules";
    config["AccountOwner"] = "hokee";
    config["ColumnNames"] = "Category;Payer/Payee;Description;Type;Date;Account;Value";
    config["HasHeader"] = "true";
    config["IgnoreLines"] = "4";
    config["HasDoubleQuotes"] = "false";
    config["HasTrailingDelimiter"] = "false";
    config["Delimiter"] = ";";
    config["DateFormat"] = "dd.mm.yyyy";
    config["Category"] = "0";
    config["PayerPayee"] = "1";
    config["Payer"] = "-1";
    config["Payee"] = "-1";
    config["Description"] = "2";
    config["Type"] = "3";
    config["Date"] = "4";
    config["Account"] = "5";
    config["Value"] = "6";
    CsvFormat format(config, "builtin CsvRules format");
    return format;
}

std::vector<std::string> CsvRules::GetCategories()
{
    auto rulesFormat = CsvRules::GetFormat();
    const auto rulesHeader = this->GetCsvHeader();
    if (rulesHeader.size() != static_cast<size_t>(rulesFormat.GetIgnoreLines()))
    {
        throw InternalException(__FILE__, __LINE__, "Internal header of rule set does not match expected format!");
    }

    return Utils::SplitLine(rulesHeader[1], rulesFormat.GetDelimiter(), rulesFormat.GetHasTrailingDelimiter());
}
} // namespace hokee