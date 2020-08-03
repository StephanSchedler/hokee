#pragma once

#include "html/IHtmlPrintable.h"

namespace hokee
{
class HtmlText final : IHtmlPrintable
{
    std::string _text{};
  public:
    HtmlText() = delete;
    HtmlText(const std::string& text);
    virtual ~HtmlText() = default;

    std::string ToString() override;
    void ToString(std::stringstream& output) override;
};

} // namespace hokee