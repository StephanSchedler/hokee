#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlTableHeader final : public HtmlElement
{  
  public:
    HtmlTableHeader()
        : HtmlElement("th"){};
    HtmlTableHeader(const std::string& text)
        : HtmlElement("th", text){};
    virtual ~HtmlTableHeader() = default;
};

} // namespace hokee