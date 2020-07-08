#include "csv/CsvDate.h"
#include "InternalException.h"
#include "Utils.h"

#include <cstdint>
#include <fmt/format.h>

namespace hokee
{
CsvDate::CsvDate(std::string_view formatStr, std::string_view dateStr)
{
    if (dateStr.empty())
    {
        return;
    }

    if (formatStr.size() != dateStr.size())
    {
        throw std::runtime_error(
            fmt::format("Could not parse date string. Format string '{}' does not match date string '{}'.",
                        formatStr, dateStr));
    }
    if (formatStr == "dd.mm.yy")
    {
        const std::string_view dayStr = dateStr.substr(0, 2);
        try
        {
            _day = std::stoi(std::string(dayStr.data(), dayStr.size()));
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(fmt::format("Could not convert '{}' to 'int'. ({})", dayStr, e.what()));
        }

        const std::string_view monthStr = dateStr.substr(3, 2);
        try
        {
            _month = std::stoi(std::string(monthStr.data(), monthStr.size()));
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(fmt::format("Could not convert '{}' to 'int'. ({})", monthStr, e.what()));
        }

        const std::string_view yearStr = dateStr.substr(6, 2);
        try
        {
            _year = std::stoi(std::string(yearStr.data(), yearStr.size()));
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(fmt::format("Could not convert '{}' to 'int'. ({})", yearStr, e.what()));
        }

        if (_year >= 70)
        {
            _year += 1900;
        }
        else
        {
            _year += 2000;
        }
    }
    else if (formatStr == "dd.mm.yyyy")
    {
        const std::string_view dayStr = dateStr.substr(0, 2);
        try
        {
            _day = std::stoi(std::string(dayStr.data(), dayStr.size()));
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(fmt::format("Could not convert '{}' to 'int'. ({})", dayStr, e.what()));
        }

        const std::string_view monthStr = dateStr.substr(3, 2);
        try
        {
            _month = std::stoi(std::string(monthStr.data(), monthStr.size()));
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(fmt::format("Could not convert '{}' to 'int'. ({})", monthStr, e.what()));
        }

        const std::string_view yearStr = dateStr.substr(6, 4);
        try
        {
            _year = std::stoi(std::string(yearStr.data(), yearStr.size()));
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(fmt::format("Could not convert '{}' to 'int'. ({})", yearStr, e.what()));
        }
    }
    else
    {
        throw UserException(fmt::format("Could not parse date string. Unsupported format string \"{}\" "
                                        "(Supported formats: \"dd.mm.yy\", \"dd.mm.yyyy\")",
                                        formatStr));
    }

    _dateStr = fmt::format("{:#02}.{:#02}.{:#04}", _day, _month, _year);
}

const std::string& CsvDate::ToString() const
{
    return _dateStr;
}

bool CsvDate::operator==(const CsvDate& rhs) const
{
    return (_day == rhs._day) && (_month == rhs._month) && (_year == rhs._year);
}

bool CsvDate::operator!=(const CsvDate& rhs) const
{
    return !(*this == rhs);
}

bool CsvDate::operator<(const CsvDate& other) const
{
    if (_year < other._year)
    {
        return true;
    }
    if (_year > other._year)
    {
        return false;
    }

    if (_month < other._month)
    {
        return true;
    }
    if (_month > other._month)
    {
        return false;
    }

    if (_day < other._day)
    {
        return true;
    }
    if (_day > other._day)
    {
        return false;
    }

    return false;
}

std::ostream& operator<<(std::ostream& os, const CsvDate& date)
{
    os << date.ToString();
    return os;
}

} // namespace hokee