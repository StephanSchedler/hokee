#include "HtmlElement.h"
#include "html/HtmlText.h"

#include <fmt/format.h>

namespace hokee
{
HtmlElement::HtmlElement(const std::string& name)
    : _printInline{false}
    , _name{name}
{
}

HtmlElement::HtmlElement(const std::string& name, const std::string& text)
    : _printInline{true}
    , _name{name}
{
    AddElement(text);
}

void HtmlElement::SetIndent(int indent)
{
    _indent = indent;
}

void HtmlElement::AddElement(std::unique_ptr<IHtmlPrintable>&& element)
{
    auto htmlElement = dynamic_cast<HtmlElement*>(element.get());
    if (htmlElement)
    {
        htmlElement->SetIndent(_indent + 2);
    }
    _elements.push_back(std::move(element));
}

void HtmlElement::AddElement(const std::string& text)
{
    _elements.push_back(std::make_unique<HtmlText>(text));
}

void HtmlElement::AddAttribute(const std::string& attributeName, const std::string& attributeValue)
{
    _attributes[attributeName] = fmt::format("\"{}\"", attributeValue);
}

void HtmlElement::ToString(std::ostream& output) const
{
    output << fmt::format("<{}", _name);
    for (auto& attribute : _attributes)
    {
        if (attribute.second.empty())
        {
            output << fmt::format(" {}", attribute.first);
        }
        else
        {
            output << fmt::format(" {}={}", attribute.first, attribute.second);
        }
    }

    if (_elements.empty())
    {
        output << "/>";
    }
    else
    {
        output << ">";
        for (auto& element : _elements)
        {
            if (!_printInline)
            {
                output << std::endl;
                output << std::string(_indent + 2, ' ');
            }
            output << *element;
        }

        if (!_printInline)
        {
            output << std::endl;
            output << std::string(_indent, ' ');
        }
        output << fmt::format("</{}>", _name);
    }
}
} // namespace hokee