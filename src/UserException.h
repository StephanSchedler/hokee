#pragma once

#include <exception>
#include <stdexcept>
#include <string>

class UserException final : public std::runtime_error
{
    public:
    UserException(const std::string& msg);
    virtual ~UserException() = default;

    UserException(const UserException&) = default;
    UserException& operator=(const UserException&) = default;
    UserException(UserException&&) = default;
    UserException& operator=(UserException&&) = default;
};