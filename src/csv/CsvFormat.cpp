#include "CsvFormat.h"

#include <fmt/format.h>

namespace hokee
{
CsvFormat::CsvFormat() 
    : CsvConfig()
{
    SetFormatName("Default");
    SetAccountOwner("John Doe");
    SetColumnNames(std::vector<std::string>{"description", "value"});
    SetHasHeader(true);
    SetIgnoreLines(0);
    SetHasDoubleQuotes(false);
    SetHasTrailingDelimiter(false);
    SetDelimiter(';');
    SetDateFormat("dd.mm.yy");
    SetCategory(-1);
    SetPayerPayee(-1);
    SetPayer(-1);
    SetPayee(-1);
    SetDescription(-1);
    SetType(-1);
    SetDate(-1);
    SetAccount(-1);
    SetValue(-1);
}

CsvFormat::CsvFormat(const fs::path& file)
    : CsvConfig(file)
{
}

} // namespace hokee