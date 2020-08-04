#pragma once

#include "HtmlBody.h"
#include "HtmlHead.h"
#include "HtmlTitle.h"
#include "HtmlText.h"
#include "HtmlHeading.h"
#include "HtmlLink.h"
#include "HtmlParagraph.h"
#include "HtmlDivision.h"
#include "HtmlHeader.h"
#include "HtmlTable.h"
#include "HtmlMain.h"
#include "HtmlBold.h"
#include "HtmlFooter.h"
#include "HtmlImage.h"
#include "HtmlBreak.h"

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