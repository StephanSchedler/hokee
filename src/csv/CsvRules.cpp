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
    config["IgnoreLines"] = "0";
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
} // namespace hokee