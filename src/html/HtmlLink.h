#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlLink final : public HtmlElement
{  
  public:
    HtmlLink()
        : HtmlElement("link"){};
    HtmlLink(const std::string& text)
        : HtmlElement("link", text){};
    virtual ~HtmlLink() = default;
};

} // namespace hokee