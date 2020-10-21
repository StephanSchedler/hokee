#pragma once

#include <iostream>
#include <string>
#include <string_view>

namespace hokee
{
class CsvValue
{
    double _value = 0;
    std::string _string = "";

  public:
    CsvValue() = default;

    CsvValue(const std::string& value, const std::string& file, int lineCounter, bool validate = true);
    ~CsvValue() = default;

    CsvValue(const CsvValue&) = default;
    CsvValue& operator=(const CsvValue&) = default;
    CsvValue(CsvValue&&) = default;
    CsvValue& operator=(CsvValue&&) = default;

    inline const std::string& ToString() const
    {
        return _string;
    };

    inline double ToDouble() const
    {
        return _value;
    };
    
    friend std::ostream& operator<<(std::ostream& os, const CsvValue& value);
};

} // namespace hokee