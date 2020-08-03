#pragma once

#include "HtmlBody.h"
#include "HtmlElement.h"
#include "HtmlHead.h"

#include <memory>

namespace hokee
{
class HtmlPage : public IHtmlPrintable
{
    std::unique_ptr<HtmlHead> _head{};
    std::unique_ptr<HtmlBody> _body{};

    void ToString(std::ostream& output) const override;

  public:
    HtmlPage() = delete;
    HtmlPage(std::unique_ptr<HtmlHead> head, std::unique_ptr<HtmlBody> body);
    virtual ~HtmlPage() = default;
};

} // namespace hokee