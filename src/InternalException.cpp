#include "InternalException.h"

#include "fmt/format.h"

namespace hokee
{
InternalException::InternalException(std::string_view file, int line, std::string_view msg)
    : std::exception()
{
    _msg = fmt::format("{}:{}: {}", file, line, msg);
}

char const* InternalException::what() const noexcept
{
    return _msg.c_str();
}
} // namespace hokee
