#pragma once

#include <vector>
#include <string>

struct CsvFormat
{
    /// Name of the Format. If the Account property is unspecified, the format name will be assigned.
    std::string FormatName{};

    /// CSV header
    std::vector<std::string> ColumnNames{};

    /// Parser checks that header in the csv file matches the ColumnNames property
    bool HasHeader{true};

    /// Parser ignores the first rows in the csv file
    int IgnoreLines{0};

    /// Specifies if the parser has to remove double quotes (") from each cell
    bool HasDoubleQuotes{false};

    /// Specifies if the parser has to remove a tailing delimter char at the end of each row
    bool HasTrailingDelimiter{false};

    /// Delimiter character inbetween columns
    char Delimiter{';'};

    /// Format of date string in the csv file
    std::string DateFormat{"dd.mm.yy"};

    // Column of Category string. (Set to -1, if it is not supported in the csv file)
    int Category{-1};

    // Column of PayerPayee string. (Set to -1, if it is not supported in the csv file)
    int PayerPayee{-1};

    // Column of Payer string. (Set to -1, if it is not supported in the csv file)
    int Payer{-1};

    // Column of Payee string. (Set to -1, if it is not supported in the csv file)
    int Payee{-1};

    // Column of Description string. (Set to -1, if it is not supported in the csv file)
    int Description{-1};

    // Column of Type string. (Set to -1, if it is not supported in the csv file)
    int Type{-1};

    // Column of Date string. (Set to -1, if it is not supported in the csv file)
    int Date{-1};

    // Column of Account string (== name of account). (Set to -1, if it is not supported in the csv file)
    int Account{-1};

    // Column of Value string. (Set to -1, if it is not supported in the csv file)
    int Value{-1};
};