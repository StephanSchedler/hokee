#include "CsvFormat.h"

#include <fmt/format.h>

namespace hokee
{
CsvFormat::CsvFormat(const fs::path& file)
    : CsvConfig(file)
{
    InitializeProperties(file);
}

CsvFormat::CsvFormat(const std::unordered_map<std::string, std::string>& config, const fs::path& file)
    : CsvConfig(config, file)
{
    InitializeProperties(file);
}

void CsvFormat::InitializeProperties(const fs::path& file)
{
    _formatName = file.parent_path().filename().string();

    _accountOwner = GetString("AccountOwner");
    _columnNames = GetStrings("ColumnNames");
    _hasHeader = GetBool("HasHeader");
    _hasDoubleQuotes = GetBool("HasDoubleQuotes");
    _hasTrailingDelimiter = GetBool("HasTrailingDelimiter");
    _delimiter = GetChar("Delimiter");
    _dateFormat = GetString("DateFormat");
    _ignoreLines = GetInt("IgnoreLines");
    _category = GetInt("Category");
    _payerPayee = GetInt("PayerPayee");
    _payer = GetInt("Payer");
    _payee = GetInt("Payee");
    _description = GetInt("Description");
    _type = GetInt("Type");
    _date = GetInt("Date");
    _account = GetInt("Account");
    _value = GetInt("Value");
}

const std::string CsvFormat::GetFormatName() const
{
    return _formatName;
}

const std::string CsvFormat::GetAccountOwner() const
{
    return _accountOwner;
}

const std::vector<std::string> CsvFormat::GetColumnNames() const
{
    return _columnNames;
}

bool CsvFormat::GetHasHeader() const
{
    return _hasHeader;
}

bool CsvFormat::GetHasDoubleQuotes() const
{
    return _hasDoubleQuotes;
}

bool CsvFormat::GetHasTrailingDelimiter() const
{
    return _hasTrailingDelimiter;
}

char CsvFormat::GetDelimiter() const
{
    return _delimiter;
}

const std::string CsvFormat::GetDateFormat() const
{
    return _dateFormat;
}

int CsvFormat::GetIgnoreLines() const
{
    return _ignoreLines;
}

int CsvFormat::GetCategory() const
{
    return _category;
}

int CsvFormat::GetPayerPayee() const
{
    return _payerPayee;
}

int CsvFormat::GetPayer() const
{
    return _payer;
}

int CsvFormat::GetPayee() const
{
    return _payee;
}

int CsvFormat::GetDescription() const
{
    return _description;
}

int CsvFormat::GetType() const
{
    return _type;
}

int CsvFormat::GetDate() const
{
    return _date;
}

int CsvFormat::GetAccount() const
{
    return _account;
}

int CsvFormat::GetValue() const
{
    return _value;
}

} // namespace hokee