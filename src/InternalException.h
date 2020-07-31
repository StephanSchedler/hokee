#pragma once

#include "UserException.h"

#include <exception>
#include <string>
#include <string_view>

namespace hokee
{
class InternalException final : public std::exception
{
    std::string _msg;

  public:
    InternalException() = delete;
    InternalException(std::string_view file, int line, std::string_view msg);
    virtual ~InternalException() = default;

    InternalException(const InternalException&) = default;
    InternalException& operator=(const InternalException&) = default;
    InternalException(InternalException&&) = default;
    InternalException& operator=(InternalException&&) = default;

    char const* what() const noexcept override;
};
} // namespace hokee
