#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlHead final : public HtmlElement
{
  public:
    HtmlHead()
        : HtmlElement("head"){};
    HtmlHead(const std::string& text)
        : HtmlElement("head", text){};
    virtual ~HtmlHead() = default;
};

} // namespace hokee