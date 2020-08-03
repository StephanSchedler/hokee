#pragma once

#include "HtmlElement.h"
#include <fmt/format.h>

namespace hokee
{
class HtmlHeading final : public HtmlElement
{  
  public:
    HtmlHeading(int size) : HtmlElement(fmt::format("h{}", size)) {};
    HtmlHeading(int size, const std::string& text) : HtmlElement(fmt::format("h{}", size), text) {};
    virtual ~HtmlHeading() = default;
};

} // namespace hokee