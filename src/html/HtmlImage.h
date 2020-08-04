#pragma once

#include "HtmlElement.h"
#include "fmt/format.h"

namespace hokee
{
class HtmlImage final : public HtmlElement
{
  public:
    HtmlImage(const std::string& src, const std::string& title, int width, int height)
        : HtmlElement("img")
    {
        AddAttribute("src", src);
        AddAttribute("title", title);
        AddAttribute("width", fmt::format("{}", width));
        AddAttribute("height", fmt::format("{}", height));
    };
    HtmlImage(const std::string& src, const std::string& title, int size)
        : HtmlImage(src, title, size, size){};
    virtual ~HtmlImage() = default;
};

} // namespace hokee