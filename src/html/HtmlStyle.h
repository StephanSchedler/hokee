#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlStyle final : public HtmlElement
{  
  public:
    HtmlStyle()
        : HtmlElement("style"){};
    HtmlStyle(const std::string& text)
        : HtmlElement("style", text){};
    virtual ~HtmlStyle() = default;
};

} // namespace hokee