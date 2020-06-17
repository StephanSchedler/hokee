#include "UserException.h"

UserException::UserException(const std::string& msg)
    : std::runtime_error(msg.c_str())
{
}
