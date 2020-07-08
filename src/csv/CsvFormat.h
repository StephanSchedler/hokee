#pragma once

#include "CsvConfig.h"

namespace hokee
{
class CsvFormat final : public CsvConfig
{
  public:
    CsvFormat();
    CsvFormat(const fs::path& file);

    CsvFormat(const CsvFormat&) = default;
    CsvFormat& operator=(const CsvFormat&) = default;
    CsvFormat(CsvFormat&&) = default;
    CsvFormat& operator=(CsvFormat&&) = default;

    /// Name of the Format. If the Account property is unspecified, the format name will be assigned.
    const std::string GetFormatName() const;

    /// Name of the account owner
    const std::string GetAccountOwner() const;

    /// CSV header
    const std::vector<std::string> GetColumnNames() const;

    /// If true, parser checks that header in the csv file matches the ColumnNames property
    bool GetHasHeader() const;

    /// Parser ignores the first rows in the csv file
    int GetIgnoreLines() const;
    
    /// Specifies if the parser has to remove double quotes (") from each cell
    bool GetHasDoubleQuotes() const;

    /// Specifies if the parser has to remove a tailing delimter char at the end of each row
    bool GetHasTrailingDelimiter() const;
    
    /// Delimiter character inbetween columns
    char GetDelimiter() const;

    /// Format of date string in the csv file
    const std::string GetDateFormat() const;
    
    // Column of Category string. (Set to -1, if it is not supported in the csv file)
    int GetCategory() const;

    // Column of PayerPayee string. (Set to -1, if it is not supported in the csv file)
    int GetPayerPayee() const;

    // Column of Payer string. (Set to -1, if it is not supported in the csv file)
    int GetPayer() const;

    // Column of Payee string. (Set to -1, if it is not supported in the csv file)
    int GetPayee() const;

    // Column of Description string. (Set to -1, if it is not supported in the csv file)
    int GetDescription() const;

    // Column of Type string. (Set to -1, if it is not supported in the csv file)
    int GetType() const;

    // Column of Date string. (Set to -1, if it is not supported in the csv file)
    int GetDate() const;

    // Column of Account string (== name of account). (Set to -1, if it is not supported in the csv file)
    int GetAccount() const;

    // Column of Value string. (Set to -1, if it is not supported in the csv file)
    int GetValue() const;

    void SetFormatName(const std::string& value);
    void SetAccountOwner(const std::string& value);
    void SetColumnNames(const std::vector<std::string>& value);
    void SetHasHeader(bool value);
    void SetIgnoreLines(int value);
    void SetHasDoubleQuotes(bool value);
    void SetHasTrailingDelimiter(bool value);
    void SetDelimiter(char value);
    void SetDateFormat(const std::string& value);
    void SetCategory(int value);
    void SetPayerPayee(int value);
    void SetPayer(int value);
    void SetPayee(int value);
    void SetDescription(int value);
    void SetType(int value);
    void SetDate(int value);
    void SetAccount(int value);
    void SetValue(int value);
};

} // namespace hokee