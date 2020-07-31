#pragma once

#include "Filesystem.h"

#include <exception>
#include <stdexcept>
#include <string>

namespace hokee
{
class UserException final : public std::runtime_error
{
    fs::path _file = "";
    int _line = 0;

  public:
    UserException(const std::string& msg, const fs::path& file = "", int line = 0);
    virtual ~UserException() = default;

    UserException(const UserException&) = default;
    UserException& operator=(const UserException&) = default;
    UserException(UserException&&) = default;
    UserException& operator=(UserException&&) = default;

    const fs::path GetFile() const;
    int GetLine() const;
};
} // namespace hokee
