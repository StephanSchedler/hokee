#include "CsvValue.h"
#include "InternalException.h"
#include "Utils.h"

#include <cstdint>
#include <fmt/format.h>

namespace hokee
{
CsvValue::CsvValue(const std::string& value, const std::string& file, int lineCounter, bool validate)
    : _string{value}
{
    // Fix value
    _string.erase(std::remove(_string.begin(), _string.end(), '_'), _string.end());
    _string.erase(std::remove(_string.begin(), _string.end(), ' '), _string.end());
    size_t pos = _string.find_last_of(".,");
    if (pos == std::string::npos)
    {
        if (!_string.empty())
        {
            _string += ".00";
        }
    }
    else
    {
        if (_string[pos] == ',')
        {
            _string.erase(std::remove(_string.begin(), _string.end(), '.'), _string.end());
            std::replace(_string.begin(), _string.end(), ',', '.');
        }
        else
        {
            _string.erase(std::remove(_string.begin(), _string.end(), ','), _string.end());
        }
    }

    // Validate value
    if (_string.empty())
    {
        return;
    }
    try
    {
        _value = std::stod(_string);
        _string = fmt::format("{:.2f}", _value);
    }
    catch (const std::exception& e)
    {
        throw InternalException(__FILE__, __LINE__,
                                fmt::format("Could not convert '{}' to 'double'. ({})", _string, e.what()));
    }
    if (validate && _string != fmt::format("{:.2f}", _value))
    {
        throw UserException(
            fmt::format("Could not parse converted value {} != stod({})", _string, fmt::format("{:.2f}", _value)),
            file, lineCounter);
    };
};

std::ostream& operator<<(std::ostream& os, const CsvValue& value)
{
    os << value.ToString();
    return os;
}
} // namespace hokee