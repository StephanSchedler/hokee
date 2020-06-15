#include "CsvDate.h"
#include "CustomException.h"

#include <cstdint>
#include <fmt/format.h>

namespace hokeeboo
{
CsvDate::CsvDate(std::string_view formatStr, std::string_view dateStr)
{
    if (dateStr.empty())
    {
        return;
    }
    
    if (formatStr.size() != dateStr.size())
    {
        throw CustomException(
            __FILE__, __LINE__,
            fmt::format("Could not parse date string. Format string size {} does not match date string size {}",
                        formatStr.size(), dateStr.size()));
    }
    if (formatStr == "dd.mm.yy")
    {
        const std::string_view dayStr = dateStr.substr(0, 2);
        _day = std::stoi(std::string(dayStr.data(), dayStr.size()));

        const std::string_view monthStr = dateStr.substr(3, 2);
        _month = std::stoi(std::string(monthStr.data(), monthStr.size()));

        const std::string_view yearStr = dateStr.substr(6, 2);
        _year = std::stoi(std::string(yearStr.data(), yearStr.size()));

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
        _day = std::stoi(std::string(dayStr.data(), dayStr.size()));

        const std::string_view monthStr = dateStr.substr(3, 2);
        _month = std::stoi(std::string(monthStr.data(), monthStr.size()));

        const std::string_view yearStr = dateStr.substr(6, 4);
        _year = std::stoi(std::string(yearStr.data(), yearStr.size()));
    }
    else
    {
        throw CustomException(__FILE__, __LINE__,
                              fmt::format("Could not parse date string. Unsupported format string \"{}\" "
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

} // namespace hokeeboo