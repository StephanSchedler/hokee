#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlBold final : public HtmlElement
{  
  public:
    HtmlBold()
        : HtmlElement("b"){};
    HtmlBold(const std::string& text)
        : HtmlElement("b", text){};
    virtual ~HtmlBold() = default;
};

} // namespace hokee