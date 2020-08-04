#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlLink final : public HtmlElement
{
  public:
    HtmlLink(const std::string& link, const std::string& title)
        : HtmlElement("a")
    {
        AddAttribute("href", link);
        AddAttribute("title", title);
    };
    HtmlLink(const std::string& link, const std::string& title, const std::string& text)
        : HtmlElement("a", text)
    {
        AddAttribute("href", link);
        AddAttribute("title", title);
    };
    virtual ~HtmlLink() = default;
};

} // namespace hokee