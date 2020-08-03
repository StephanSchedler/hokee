#pragma once

#include "HtmlElement.h"
#include <fmt/format.h>

namespace hokee
{
class HtmlTitle final : public HtmlElement
{
  public:
    HtmlTitle() = delete;
    HtmlTitle(const std::string& text)
        : HtmlElement("title", text){};
    virtual ~HtmlTitle() = default;
};

} // namespace hokee