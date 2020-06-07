#pragma once

#include <exception>
#include <string>
#include <string_view>

class CustomException final : public std::exception
{
    std::string _msg;

  public:
    CustomException() = delete;
    CustomException(std::string_view file, int line, std::string_view msg);
    virtual ~CustomException() = default;

    CustomException(const CustomException&) = default;
    CustomException& operator=(const CustomException&) = default;
    CustomException(CustomException&&) = default;
    CustomException& operator=(CustomException&&) = default;

    char const* what() const noexcept override;
};