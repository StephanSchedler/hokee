#pragma once
#include <string>
#include <sstream>

namespace hokee
{
class IHtmlPrintable
{
  public:
    virtual ~IHtmlPrintable() = default;    
    virtual std::string ToString() = 0;
    virtual void ToString(std::stringstream& output) = 0;
};

} // namespace hokee