#include "HtmlText.h"

namespace hokee
{
HtmlText::HtmlText(const std::string& text)
    : _text{text}
{
}

void HtmlText::ToString(std::ostream& output) const
{
    output << _text;
}
} // namespace hokee