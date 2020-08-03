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

void HtmlElement::SetElements(std::vector<std::unique_ptr<IHtmlPrintable>>&& elements)
{
    _elements = std::move(elements);
}

void HtmlElement::AddElement(std::unique_ptr<IHtmlPrintable>&& element)
{
    _elements.push_back(std::move(element));
}

void HtmlElement::AddElement(const std::string& text)
{
    _elements.push_back(std::make_unique<HtmlText>(text));
}

void HtmlElement::AddAttribute(const std::string& attributeName, const std::string& attributeValue)
{
    _attributes[attributeName] = attributeValue;
}

void HtmlElement::ToString(std::ostream& output) const
{
    if (!_printInline)
    {
        output << std::string(_indent, ' ');
    }
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
        if (!_printInline)
        {
            output << std::endl;
        }
    }
    else
    {
        output << ">";
        if (!_printInline)
        {
            output << std::endl;
        }
        for (auto& element : _elements)
        {
            output << *element;
        }

        if (!_printInline)
        {
            output << std::string(_indent, ' ');
        }
        output << fmt::format("</{}>", _name);
        if (!_printInline)
        {
            output << std::endl;
        }
    }
}
} // namespace hokee