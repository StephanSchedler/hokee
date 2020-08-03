#include "HtmlText.h"

namespace hokee
{
HtmlText::HtmlText(const std::string& text)
    : _text{text}
{
}

void HtmlText::ToString(std::ostream& output) const
{
    for (size_t pos = 0; pos != _text.size(); ++pos)
    {
        switch (_text[pos])
        {
            case '&':
                output << "&amp;";
                break;
            case '\"':
                output << "&quot;";
                break;
            case '\'':
                output << "&apos;";
                break;
            case '<':
                output << "&lt;";
                break;
            case '>':
                output << "&gt;";
                break;
            default:
                output << _text[pos];
                break;
        }
    }
}
} // namespace hokee