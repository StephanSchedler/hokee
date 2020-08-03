#pragma once

#include "HtmlElement.h"

#include <vector>
#include <unordered_map>
#include <memory>

namespace hokee
{
class HtmlBreak final : HtmlElement
{  
  public:
    HtmlBreak() : HtmlElement("br", true) {};
    virtual ~HtmlBreak() = default;
};

} // namespace hokee