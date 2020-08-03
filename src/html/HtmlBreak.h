#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlBreak final : public HtmlElement
{  
  public:
    HtmlBreak() : HtmlElement("br") {};
    virtual ~HtmlBreak() = default;
};

} // namespace hokee