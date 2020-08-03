#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlBody final : public HtmlElement
{
  public:
    HtmlBody()
        : HtmlElement("body"){};
    HtmlBody(const std::string& text)
        : HtmlElement("body", text){};
    virtual ~HtmlBody() = default;
};

} // namespace hokee