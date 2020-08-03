#pragma once

#include "HtmlElement.h"

namespace hokee
{
class HtmlMain final : public HtmlElement
{  
  public:
    HtmlMain()
        : HtmlElement("main"){};
    HtmlMain(const std::string& text)
        : HtmlElement("main", text){};
    virtual ~HtmlMain() = default;
};

} // namespace hokee