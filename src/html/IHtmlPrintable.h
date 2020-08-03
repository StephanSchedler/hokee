#pragma once
#include <iostream>
#include <sstream>
#include <string>

namespace hokee
{
class IHtmlPrintable
{
    friend std::ostream& operator<<(std::ostream& os, const IHtmlPrintable& obj);

    virtual void ToString(std::ostream& output) const = 0;

  public:
    virtual ~IHtmlPrintable() = default;
};

} // namespace hokee