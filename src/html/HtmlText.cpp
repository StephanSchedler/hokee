#include "HtmlText.h"

namespace hokee
{
HtmlText::HtmlText(const std::string& text)
{
    _text.reserve(static_cast<size_t>(text.size()*1.1));
    for (size_t pos = 0; pos != text.size(); ++pos)
    {
        switch (text[pos])
        {
            case '&':
                _text.append("&amp;");
                break;
            case '\"':
                _text.append("&quot;");
                break;
            case '\'':
                _text.append("&apos;");
                break;
            case '<':
                _text.append("&lt;");
                break;
            case '>':
                _text.append("&gt;");
                break;
            default:
                _text.append(&text[pos], 1);
                break;
        }
    }
}

std::string HtmlText::ToString()
{
    return _text;
}

void HtmlText::ToString(std::stringstream& output)
{
    output << _text;
}
} // namespace hokee