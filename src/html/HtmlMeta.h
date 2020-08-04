#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlMeta final : public HtmlElement
{  
  public:
    HtmlMeta()
        : HtmlElement("meta"){};
    HtmlMeta(const std::string& text)
        : HtmlElement("meta", text){};
    virtual ~HtmlMeta() = default;
};

} // namespace hokee