#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlHyperlink final : public HtmlElement
{
  public:
    HtmlHyperlink(const std::string& link, const std::string& title)
        : HtmlElement("a")
    {
        AddAttribute("href", link);
        AddAttribute("title", title);
    };
    HtmlHyperlink(const std::string& link, const std::string& title, const std::string& text)
        : HtmlElement("a", text)
    {
        AddAttribute("href", link);
        AddAttribute("title", title);
    };
    virtual ~HtmlHyperlink() = default;
};

} // namespace hokee