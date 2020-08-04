#pragma once

#include "HtmlElement.h"
#include "HtmlTableHeader.h"
#include "HtmlTableCell.h"
#include "HtmlTableRow.h"

namespace hokee
{
class HtmlTable final : public HtmlElement
{  
  public:
    HtmlTable()
        : HtmlElement("table"){};
    HtmlTable(const std::string& text)
        : HtmlElement("table", text){};
    virtual ~HtmlTable() = default;
};

} // namespace hokee