#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlTableRow final : public HtmlElement
{  
  public:
    HtmlTableRow()
        : HtmlElement("tr"){};
    HtmlTableRow(const std::string& text)
        : HtmlElement("tr", text){};
    virtual ~HtmlTableRow() = default;
};

} // namespace hokee