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

const std::string CsvFormat::GetFormatName() const
{
    return GetString("FormatName");
}

const std::string CsvFormat::GetAccountOwner() const
{
    return GetString("AccountOwner");
}

const std::vector<std::string> CsvFormat::GetColumnNames() const
{
    return GetStrings("ColumnNames");
}

bool CsvFormat::GetHasHeader() const
{
    return GetBool("HasHeader");
}

bool CsvFormat::GetHasDoubleQuotes() const
{
    return GetBool("HasDoubleQuotes");
}

bool CsvFormat::GetHasTrailingDelimiter() const
{
    return GetBool("HasTrailingDelimiter");
}

char CsvFormat::GetDelimiter() const
{
    return GetChar("Delimiter");
}

const std::string CsvFormat::GetDateFormat() const
{
    return GetString("DateFormat");
}

int CsvFormat::GetIgnoreLines() const
{
    return GetInt("IgnoreLines");
}

int CsvFormat::GetCategory() const
{
    return GetInt("Category");
}

int CsvFormat::GetPayerPayee() const
{
    return GetInt("PayerPayee");
}

int CsvFormat::GetPayer() const
{
    return GetInt("Payer");
}

int CsvFormat::GetPayee() const
{
    return GetInt("Payee");
}

int CsvFormat::GetDescription() const
{
    return GetInt("Description");
}

int CsvFormat::GetType() const
{
    return GetInt("Type");
}

int CsvFormat::GetDate() const
{
    return GetInt("Date");
}

int CsvFormat::GetAccount() const
{
    return GetInt("Account");
}

int CsvFormat::GetValue() const
{
    return GetInt("Value");
}

void CsvFormat::SetFormatName(const std::string& value)
{
    SetString("FormatName", value);
}

void CsvFormat::SetAccountOwner(const std::string& value)
{
    SetString("AccountOwner", value);
}

void CsvFormat::SetColumnNames(const std::vector<std::string>& value)
{
    SetStrings("ColumnNames", value);
}

void CsvFormat::SetHasHeader(bool value)
{
    SetBool("HasHeader", value);
}

void CsvFormat::SetHasDoubleQuotes(bool value)
{
    SetBool("HasDoubleQuotes", value);
}

void CsvFormat::SetHasTrailingDelimiter(bool value)
{
    SetBool("HasTrailingDelimiter", value);
}

void CsvFormat::SetDelimiter(char value)
{
    SetChar("Delimiter", value);
}

void CsvFormat::SetDateFormat(const std::string& value)
{
    SetString("DateFormat", value);
}

void CsvFormat::SetIgnoreLines(int value)
{
    SetInt("IgnoreLines", value);
}

void CsvFormat::SetCategory(int value)
{
    SetInt("Category", value);
}

void CsvFormat::SetPayerPayee(int value)
{
    SetInt("PayerPayee", value);
}

void CsvFormat::SetPayer(int value)
{
    SetInt("Payer", value);
}

void CsvFormat::SetPayee(int value)
{
    SetInt("Payee", value);
}

void CsvFormat::SetDescription(int value)
{
    SetInt("Description", value);
}

void CsvFormat::SetType(int value)
{
    SetInt("Type", value);
}

void CsvFormat::SetDate(int value)
{
    SetInt("Date", value);
}

void CsvFormat::SetAccount(int value)
{
    SetInt("Account", value);
}

void CsvFormat::SetValue(int value)
{
    SetInt("Value", value);
}

} // namespace hokee