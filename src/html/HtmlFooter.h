#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlFooter final : public HtmlElement
{
  public:
    HtmlFooter()
        : HtmlElement("footer"){};
    HtmlFooter(const std::string& text)
        : HtmlElement("footer", text){};
    virtual ~HtmlFooter() = default;
};

} // namespace hokee