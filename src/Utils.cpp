#include "Utils.h"
#include "CustomException.h"

#include <fmt/format.h>

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Utils
{
const CsvFormat GetCsvFormat(const std::string& formatName)
{
    CsvFormat format{};
    format.FormatName = formatName;
    if (formatName == "Settings")
    {
        format.ColumnNames = {"Setting", "Value"};
        format.HasHeader = false;
        format.IgnoreLines = 0;
        format.HasDoubleQuotes = false;
        format.HasTrailingDelimiter = false;
        format.Delimiter = '=';
        format.Category = 0;
        format.Description = 1;
    }
    else if (formatName == "Rules")
    {
        format.ColumnNames = {"Category", "Payer/Payee", "Description", "Type", "Date", "Account", "Value"};
        format.HasHeader = true;
        format.IgnoreLines = 0;
        format.HasDoubleQuotes = false;
        format.HasTrailingDelimiter = false;
        format.Delimiter = ';';
        format.DateFormat = "dd.mm.yyyy";
        format.Category = 0;
        format.PayerPayee = 1;
        format.Payer = -1;
        format.Payee = -1;
        format.Description = 2;
        format.Type = 3;
        format.Date = 4;
        format.Account = 5;
        format.Value = 6;
    }
    else if (formatName == "ABC")
    {
        format.ColumnNames = {"date", "description", "payer/payee", "value"};
        format.HasHeader = true;
        format.IgnoreLines = 2;
        format.HasDoubleQuotes = true;
        format.HasTrailingDelimiter = false;
        format.Delimiter = ';';
        format.DateFormat = "dd.mm.yyyy";
        format.Category = -1;
        format.PayerPayee = 2;
        format.Payer = -1;
        format.Payee = -1;
        format.Description = 1;
        format.Type = -1;
        format.Date = 0;
        format.Account = -1;
        format.Value = 3;
    }
    else if (formatName == "DKB")
    {
        format.ColumnNames = {"Umsatz abgerechnet und nicht im Saldo enthalten",
                              "Wertstellung",
                              "Belegdatum",
                              "Beschreibung",
                              "Betrag (EUR)",
                              "Urspr__nglicher Betrag"};
        format.HasHeader = true;
        format.IgnoreLines = 7;
        format.HasDoubleQuotes = true;
        format.HasTrailingDelimiter = true;
        format.Delimiter = ';';
        format.DateFormat = "dd.mm.yyyy";
        format.Category = -1;
        format.PayerPayee = -1;
        format.Payer = -1;
        format.Payee = -1;
        format.Description = 3;
        format.Type = -1;
        format.Date = 2;
        format.Account = -1;
        format.Value = 4;
    }
    else if (formatName == "Postbank")
    {
        format.ColumnNames = {"Buchungstag",  "Wertstellung", "Umsatzart",  "Buchungsdetails",
                              "Auftraggeber", "Empf_nger",    "Betrag (_)", "Saldo (_)"};
        format.HasHeader = true;
        format.IgnoreLines = 8;
        format.HasDoubleQuotes = true;
        format.HasTrailingDelimiter = false;
        format.Delimiter = ';';
        format.DateFormat = "dd.mm.yyyy";
        format.Category = -1;
        format.PayerPayee = -1;
        format.Payer = 4;
        format.Payee = 5;
        format.Description = 3;
        format.Type = 2;
        format.Date = 0;
        format.Account = -1;
        format.Value = 6;
    }
    else if (formatName == "OSPA")
    {
        format.ColumnNames = {"Auftragskonto",
                              "Buchungstag",
                              "Valutadatum",
                              "Buchungstext",
                              "Verwendungszweck",
                              "Beguenstigter/Zahlungspflichtiger",
                              "Kontonummer",
                              "BLZ",
                              "Betrag",
                              "Waehrung",
                              "Info"};
        format.HasHeader = true;
        format.IgnoreLines = 0;
        format.HasDoubleQuotes = true;
        format.HasTrailingDelimiter = false;
        format.Delimiter = ';';
        format.DateFormat = "dd.mm.yy";
        format.Category = -1;
        format.PayerPayee = 5;
        format.Payer = -1;
        format.Payee = -1;
        format.Description = 4;
        format.Type = 3;
        format.Date = 1;
        format.Account = 0;
        format.Value = 8;
    }
    else if (formatName == "OSPA2")
    {
        format.ColumnNames = {"Auftragskonto",
                              "Buchungstag",
                              "Valutadatum",
                              "Buchungstext",
                              "Verwendungszweck",
                              "Glaeubiger ID",
                              "Mandatsreferenz",
                              "Kundenreferenz (End-to-End)",
                              "Sammlerreferenz",
                              "Lastschrift Ursprungsbetrag",
                              "Auslagenersatz Ruecklastschrift",
                              "Beguenstigter/Zahlungspflichtiger",
                              "Kontonummer/IBAN",
                              "BIC (SWIFT-Code)",
                              "Betrag",
                              "Waehrung",
                              "Info"};
        format.HasHeader = true;
        format.IgnoreLines = 0;
        format.HasDoubleQuotes = true;
        format.HasTrailingDelimiter = false;
        format.Delimiter = ';';
        format.DateFormat = "dd.mm.yy";
        format.Category = -1;
        format.PayerPayee = 11;
        format.Payer = -1;
        format.Payee = -1;
        format.Description = 4;
        format.Type = 3;
        format.Date = 1;
        format.Account = 0;
        format.Value = 14;
    }

    return format;
}

bool ExtractMissingString(std::string& extracted, const std::string& original, const std::string& missing)
{
    if (missing.size() > original.size())
    {
        throw CustomException(__FILE__, __LINE__,
                              "ExtractMissingString(): 'original' must not be shorter than 'missing'!");
    }

    size_t s = 0;
    while (true)
    {
        if (s < missing.size() && missing[s] == original[s])
        {
            ++s;
        }
        else
        {
            break;
        }
    }

    size_t eo = original.size() - 1;
    size_t em = missing.size() - 1;
    while (em != ~0ull && missing[em] == original[eo])
    {
        --eo;
        --em;
    }

    extracted = original.substr(s, eo - s + 1);
    return !extracted.empty();
}

int GetUniqueId()
{
    static int uniqueId = 0;
    return ++uniqueId;
}

void PrintInfo(std::string_view msg)
{
    std::cout << msg << std::endl;
}

void PrintWarning(std::string_view msg)
{
    std::cout << "WARN:  " << msg << std::endl;
}

void PrintError(std::string_view msg)
{
    std::cerr << std::endl;
    std::cerr << "ERROR: " << msg << std::endl;
}

char AskYesNoQuestion(const std::string& question, bool defaultYes)
{
    Utils::PrintInfo("");
    char answer;
    if (defaultYes)
    {
        answer = 'Y';
        Utils::PrintInfo(fmt::format("{} [Y/n]", question));
    }
    else
    {
        answer = 'N';
        Utils::PrintInfo(fmt::format("{} [y/N]", question));
    }

    std::string input;
    std::getline(std::cin, input);
    if (!input.empty())
    {
        std::istringstream stream(input);
        stream >> answer;
    }
    return answer;
}

std::string Run(const char* cmd)
{
    std::array<char, 256> buffer;
    std::string result;

#ifdef _WIN32
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
#endif
    if (!pipe)
    {
        Utils::PrintError(fmt::format("popen({}) failed!", cmd));
        return "";
    }
    while (std::fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}
} // namespace Utils