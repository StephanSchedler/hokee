#pragma once
#include <iostream>
#include <sstream>
#include <string>

namespace hokee
{
class IPrintable
{
    friend std::ostream& operator<<(std::ostream& os, const IPrintable& obj);

    virtual void ToString(std::ostream& output) const = 0;

  public:
    virtual ~IPrintable() = default;
};

} // namespace hokee