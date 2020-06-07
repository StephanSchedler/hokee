#include "CustomException.h"

#include "fmt/format.h"

CustomException::CustomException(std::string_view file, int line, std::string_view msg)
    : std::exception()
{
    _msg = fmt::format("{}:{}: {}", file, line, msg);
}

char const* CustomException::what() const noexcept 
{
    return _msg.c_str();
}