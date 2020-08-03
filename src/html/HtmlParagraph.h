#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlParagraph final : public HtmlElement
{
  public:
    HtmlParagraph()
        : HtmlElement("p"){};
    HtmlParagraph(const std::string& text)
        : HtmlElement("p", text){};
    virtual ~HtmlParagraph() = default;
};

} // namespace hokee