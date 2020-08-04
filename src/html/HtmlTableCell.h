#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlTableCell final : public HtmlElement
{  
  public:
    HtmlTableCell()
        : HtmlElement("td"){};
    HtmlTableCell(const std::string& text)
        : HtmlElement("td", text){};
    virtual ~HtmlTableCell() = default;
};

} // namespace hokee