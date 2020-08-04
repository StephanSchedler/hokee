#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlLink final : public HtmlElement
{
  public:
    HtmlLink(const std::string& link, const std::string& tooltip)
        : HtmlElement("a")
    {
        AddAttribute("href", link);
        AddAttribute("title", tooltip);
    };
    HtmlLink(const std::string& link, const std::string& tooltip, const std::string& text)
        : HtmlElement("a", text)
    {
        AddAttribute("href", link);
        AddAttribute("title", tooltip);
    };
    virtual ~HtmlLink() = default;
};

} // namespace hokee