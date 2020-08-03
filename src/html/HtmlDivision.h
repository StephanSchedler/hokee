#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlDivision final : public HtmlElement
{
  public:
    HtmlDivision()
        : HtmlElement("div"){};
    HtmlDivision(const std::string& text)
        : HtmlElement("div", text){};
    virtual ~HtmlDivision() = default;
};

} // namespace hokee