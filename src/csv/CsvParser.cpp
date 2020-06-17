#include "csv/CsvParser.h"
#include "InternalException.h"
#include "Utils.h"

#include <algorithm>
#include <fmt/core.h>
#include <fmt/format.h>

#include <memory>
#include <sstream>
#include <string>
#include <string_view>

namespace hokee
{
CsvParser::CsvParser(const fs::path& file, const CsvFormat& format, const std::string accountOwner)
    : _file{file}
    , _ifstream{file}
    , _format{format}
    , _accountOwner{accountOwner}
{
    if (_ifstream.fail())
    {
        throw InternalException(__FILE__, __LINE__, fmt::format("Could not open file {}", file.string()));
    }
}

void CsvParser::ValidateValue(const std::string value)
{
    if (value.empty())
    {
        return;
    }
    if (value != fmt::format("{:.2f}", std::stod(value)))
    {
        throw UserException(fmt::format("{}:{}: Could not parse value {} != stod({})", _file.string(),
                                        _lineCounter, value, fmt::format("{:.2f}", std::stod(value))));
    }
}

void CsvParser::Load(CsvTable& csvData)
{
    auto item = std::make_shared<CsvItem>();

    std::string line;
    std::vector<std::string> header{};
    while (std::getline(_ifstream, line) && ++_lineCounter <= _format.IgnoreLines)
    {
        header.push_back(line);
    }
    csvData.SetCsvHeader(std::move(header));

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
        throw UserException(
            fmt::format("{}:{}: Could not parse file. There is no column {}", _file.string(), _lineCounter, id));
    }
    value = cells[id];
}

bool CsvParser::GetItem(CsvRowShared& item)
{
    std::string line;
    while (std::getline(_ifstream, line))
    {
        ++_lineCounter;

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

        std::vector<std::string> cells = Utils::SplitLine(line, _format);
        if (_format.ColumnNames.size() != cells.size())
        {
            throw UserException(
                fmt::format("{}:{}: Could not parse file. Line does not match expected column count ({} != {})",
                            _file.string(), _lineCounter, _format.ColumnNames.size(), cells.size()));
        }

        std::vector<std::string> trimmedCells = {};
        for (auto& cell : cells)
        {
            if (cell.size() == 0)
            {
                trimmedCells.push_back("");
                continue;
            }
            if (!_format.HasDoubleQuotes)
            {
                trimmedCells.push_back(cell);
            }
            else
            {
                if (cell.size() < 2)
                {
                    throw UserException(fmt::format("{}:{}: Could not parse file. Cells with double quotes "
                                                    "formats must be empty or must have 2 char, at least.",
                                                    _file.string(), _lineCounter));
                }
                if (cell[0] != '"')
                {
                    throw UserException(fmt::format(
                        "{}:{}: Could not parse file. Cell '{}' does not start with double quotes '\"'",
                        _file.string(), _lineCounter, cell));
                }
                if (cell[cell.size() - 1] != '"')
                {
                    throw UserException(
                        fmt::format("{}:{}: Could not parse file. Cell '{}' does not end with double quotes '\"'",
                                    _file.string(), _lineCounter, cell));
                }

                trimmedCells.push_back(cell.substr(1, cell.size() - 2));
            }
        }

        // Check header
        if (_format.HasHeader && _lineCounter == _format.IgnoreLines + 1)
        {
            for (size_t i = 0; i < trimmedCells.size(); ++i)
            {
                if (trimmedCells[i] != _format.ColumnNames[i])
                {
                    throw UserException(fmt::format(
                        "{}:{}: Could not parse file. Header column {} does not match expected column {}",
                        _file.string(), _lineCounter, trimmedCells[i], _format.ColumnNames[i]));
                }
            }
            continue;
        }

        AssignValue(item->Account, trimmedCells, _format.Account);
        AssignValue(item->Value, trimmedCells, _format.Value);
        AssignValue(item->Category, trimmedCells, _format.Category);
        AssignValue(item->Description, trimmedCells, _format.Description);
        AssignValue(item->Type, trimmedCells, _format.Type);
        AssignValue(item->Payer, trimmedCells, _format.Payer);
        AssignValue(item->Payee, trimmedCells, _format.Payee);
        AssignValue(item->PayerPayee, trimmedCells, _format.PayerPayee);

        std::string dateStr;
        AssignValue(dateStr, trimmedCells, _format.Date);

        try
        {
            item->Date = CsvDate(_format.DateFormat, dateStr);
        }
        catch (const std::exception& e)
        {
            throw UserException(fmt::format("{}:{}: {}", _file.string(), _lineCounter, e.what()));
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
    if (_format.Account < 0)
    {
        if (_accountOwner.empty())
        {
            item->Account = fmt::format("{}", _format.FormatName);
        }
        else
        {
            item->Account = fmt::format("{} ({})", _format.FormatName, _accountOwner);
        }
    }

    // Fix value
    item->Value.erase(std::remove(item->Value.begin(), item->Value.end(), '_'), item->Value.end());
    item->Value.erase(std::remove(item->Value.begin(), item->Value.end(), ' '), item->Value.end());
    size_t pos = item->Value.find_last_of(".,");
    if (pos == std::string::npos)
    {
        if (!item->Value.empty())
        {
            item->Value += ".00";
        }
    }
    else
    {
        if (item->Value[pos] == ',')
        {
            item->Value.erase(std::remove(item->Value.begin(), item->Value.end(), '.'), item->Value.end());
            std::replace(item->Value.begin(), item->Value.end(), ',', '.');
        }
        else
        {
            item->Value.erase(std::remove(item->Value.begin(), item->Value.end(), ','), item->Value.end());
        }
    }
    ValidateValue(item->Value);

    // The csv file may contain separate Payer and Payee columns, or a single
    // PayerPayee column. As Payer or Payee is always the account owner, the
    // two columns can always be merged into a single column
    if (_format.PayerPayee < 0)
    {
        if (_format.Payer < 0 && _format.Payee >= 0)
        {
            // Assign Payee
            item->PayerPayee = item->Payee;
        }
        else if (_format.Payer >= 0 && _format.Payee < 0)
        {
            // Assign Payer
            item->PayerPayee = item->Payer;
        }
        else if (_format.Payer >= 0 && _format.Payee >= 0)
        {
            // Merge Payer/Payee by not selecting the owner
            // (Assumption: Payer or Payee equals _accountOwner)
            if (item->Payer == _accountOwner)
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
        if (_format.Payer >= 0 || _format.Payee >= 0)
        {
            throw UserException(fmt::format("{}:{}: Could not parse file. Invalid CSV format {}. You must not "
                                            "specify 'PayerPayee' along with 'Payer' or 'Payee'",
                                            _file.string(), _lineCounter, _format.FormatName)
                                    .c_str());
        }
    }

    // Set further parameter
    item->Id = Utils::GetUniqueId();
    item->File = _file;

    return result;
}

} // namespace hokee