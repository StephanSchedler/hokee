#pragma once

#include "html/IPrintable.h"

namespace hokee
{
class HtmlText final : public IPrintable
{
    std::string _text{};

    void ToString(std::ostream& output) const override;

  public:
    HtmlText() = delete;
    HtmlText(const std::string& text);
    virtual ~HtmlText() = default;
};

} // namespace hokee