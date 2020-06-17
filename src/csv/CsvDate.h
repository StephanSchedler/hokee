#pragma once

#include <iostream>
#include <string_view>
#include <string>

namespace hokee
{
class CsvDate
{
    int _year{-1};
    int _month{-1};
    int _day{-1};
    std::string _dateStr{};

  public:
    CsvDate() = default;

    /// Valid format strings: "dd.mm.yy", "dd.mm.yyyy"
    CsvDate(std::string_view formatStr, std::string_view dateStr);
    ~CsvDate() = default;

    CsvDate(const CsvDate&) = default;
    CsvDate& operator=(const CsvDate&) = default;
    CsvDate(CsvDate&&) = default;
    CsvDate& operator=(CsvDate&&) = default;

    bool operator==(const CsvDate& rhs) const;
    bool operator!=(const CsvDate& rhs) const;
    bool operator<(const CsvDate& other) const;

    inline int GetMonth()
    {
        return _month;
    }

    inline int GetYear()
    {
        return _year;
    }

    inline int GetDay()
    {
        return _day;
    }

    const std::string& ToString() const;

    friend std::ostream& operator<<(std::ostream& os, const CsvDate& date);
};

} // namespace hokee