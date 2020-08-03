#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlHeader final : public HtmlElement
{  
  public:
    HtmlHeader() : HtmlElement("header") {};
    HtmlHeader(const std::string& text) : HtmlElement("header", text) {};
    virtual ~HtmlHeader() = default;
};

} // namespace hokee