#include "CsvParser.h"
#include "InternalException.h"
#include "Utils.h"
#include "CsvValue.h"

#include <algorithm>
#include <fmt/core.h>
#include <fmt/format.h>

#include <memory>
#include <sstream>
#include <string>
#include <string_view>

namespace hokee
{
CsvParser::CsvParser(const fs::path& file, const CsvFormat& format)
    : _file{file}
    , _ifstream{file}
    , _format{format}
{
    if (_ifstream.fail())
    {
        throw InternalException(__FILE__, __LINE__, fmt::format("Could not open file {}", file.string()));
    }
}

void CsvParser::Load(CsvTable& csvData)
{
    std::string line;
    std::vector<std::string> header{};
    while (_lineCounter < _format.GetIgnoreLines())
    {
        _lineCounter++;
        std::getline(_ifstream, line);
        header.push_back(line);
    }
    csvData.SetCsvHeader(std::move(header));

    auto item = std::make_shared<CsvItem>();
    while (ParseItem(item))
    {
        csvData.push_back(item);
        item = std::make_shared<CsvItem>();
    }
}

void CsvParser::AssignValue(std::string& value, std::vector<std::string> cells, size_t id)
{
    if (id == static_cast<size_t>(~0))
    {
        return;
    }

    if (id >= cells.size())
    {
        throw UserException(fmt::format("Could not parse file. There is no column {}", id), _file, _lineCounter);
    }
    value = cells[id];
}

bool CsvParser::GetItem(CsvRowShared& item)
{
    std::string line;
    while (std::getline(_ifstream, line))
    {
        ++_lineCounter;

        if (line.empty())
        {
            continue;
        }

        // replace nonprintable characters
        for (auto& c : line)
        {
            if (c == '\n' || c == '\r' || c == '\0')
            {
                continue;
            }
            if (c == '\t')
            {
                c = '_';
            }
            const auto uc = static_cast<unsigned char>(c);
            if (uc < 32 || uc > 126)
            {
                c = '_';
            }
        }

        std::vector<std::string> cells
            = Utils::SplitLine(line, _format.GetDelimiter(), _format.GetHasTrailingDelimiter());
        if (_format.GetColumnNames().size() != cells.size())
        {
            throw UserException(
                fmt::format("Could not parse file. Line does not match expected column count ({} != {})",
                            _format.GetColumnNames().size(), cells.size()),
                _file, _lineCounter);
        }

        std::vector<std::string> trimmedCells = {};
        for (auto& cell : cells)
        {
            if (cell.size() == 0)
            {
                trimmedCells.push_back("");
                continue;
            }
            if (!_format.GetHasDoubleQuotes())
            {
                trimmedCells.push_back(cell);
            }
            else
            {
                if (cell.size() < 2)
                {
                    throw UserException(fmt::format("Could not parse file. Cells with double quotes "
                                                    "formats must be empty or must have 2 char, at least."),
                                        _file.string(), _lineCounter);
                }
                if (cell[0] != '"')
                {
                    throw UserException(
                        fmt::format("Could not parse file. Cell '{}' does not start with double quotes '\"'",
                                    cell),
                        _file, _lineCounter);
                }
                if (cell[cell.size() - 1] != '"')
                {
                    throw UserException(
                        fmt::format("Could not parse file. Cell '{}' does not end with double quotes '\"'", cell),
                        _file, _lineCounter);
                }

                trimmedCells.push_back(cell.substr(1, cell.size() - 2));
            }
        }

        // Check header
        if (_format.GetHasHeader() && _lineCounter == _format.GetIgnoreLines() + 1)
        {
            auto columnNames = _format.GetColumnNames();
            for (size_t i = 0; i < trimmedCells.size(); ++i)
            {
                if (trimmedCells[i] != columnNames[i])
                {
                    throw UserException(
                        fmt::format("Could not parse file. Header column {} does not match expected column {}",
                                    trimmedCells[i], columnNames[i]),
                        _file, _lineCounter);
                }
            }
            continue;
        }

        AssignValue(item->Account, trimmedCells, _format.GetAccount());
        std::string value;
        AssignValue(value, trimmedCells, _format.GetValue());
        item->Value = CsvValue(value, _file.string(), _lineCounter);
        
        AssignValue(item->Category, trimmedCells, _format.GetCategory());
        AssignValue(item->Description, trimmedCells, _format.GetDescription());
        AssignValue(item->Type, trimmedCells, _format.GetType());
        AssignValue(item->Payer, trimmedCells, _format.GetPayer());
        AssignValue(item->Payee, trimmedCells, _format.GetPayee());
        AssignValue(item->PayerPayee, trimmedCells, _format.GetPayerPayee());
        item->Line = _lineCounter;

        std::string dateStr;
        AssignValue(dateStr, trimmedCells, _format.GetDate());

        try
        {
            item->Date = CsvDate(_format.GetDateFormat(), dateStr);
        }
        catch (const std::exception& e)
        {
            throw UserException(e.what(), _file, _lineCounter);
        }
        return true;
    }

    return false;
}

bool CsvParser::ParseItem(CsvRowShared& item)
{
    const bool result = GetItem(item);
    if (!result)
    {
        return false;
    }

    // Set Account name
    if (_format.GetAccount() < 0)
    {
        item->Account = fmt::format("{} ({})", _format.GetFormatName(), _format.GetAccountOwner());
    }

    // The csv file may contain separate Payer and Payee columns, or a single
    // PayerPayee column. As Payer or Payee is always the account owner, the
    // two columns can always be merged into a single column
    if (_format.GetPayerPayee() < 0)
    {
        if (_format.GetPayer() < 0 && _format.GetPayee() >= 0)
        {
            // Assign Payee
            item->PayerPayee = item->Payee;
        }
        else if (_format.GetPayer() >= 0 && _format.GetPayee() < 0)
        {
            // Assign Payer
            item->PayerPayee = item->Payer;
        }
        else if (_format.GetPayer() >= 0 && _format.GetPayee() >= 0)
        {
            // Merge Payer/Payee by not selecting the owner
            // (Assumption: Payer or Payee equals AccountOwner)
            if (item->Payer == _format.GetAccountOwner())
            {
                item->PayerPayee = item->Payee;
            }
            else
            {
                item->PayerPayee = item->Payer;
            }
        }
    }
    else
    {
        if (_format.GetPayer() >= 0 || _format.GetPayee() >= 0)
        {
            throw UserException(fmt::format("Could not parse file. Invalid CSV format {}. You must not "
                                            "specify 'PayerPayee' along with 'Payer' or 'Payee'",
                                            _format.GetFormatName()),
                                _file, _lineCounter);
        }
    }

    // Set further parameter
    item->Id = Utils::GenerateId();
    item->File = _file;

    return result;
}

} // namespace hokee