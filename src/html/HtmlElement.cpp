#include "HtmlElement.h"

#include <fmt/format.h>

namespace hokee
{
HtmlElement::HtmlElement(const std::string& name, bool printInline)
    : _printInline{printInline}
    , _name{name}
{
}

void HtmlElement::SetIndent(int indent)
{
    _indent = indent;
}

void HtmlElement::AddElement(std::unique_ptr<HtmlElement>&& element)
{
    element->SetIndent(_indent + 2);
    _elements.push_back(std::move(element));
}

void HtmlElement::AddAttribute(const std::string& attributeName, const std::string& attributeValue)
{
    _attributes[attributeName] = attributeValue;
}

std::string HtmlElement::ToString()
{
    std::stringstream output;
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
            element->ToString(output);
        }

        output << std::string(_indent, ' ');
        output << fmt::format("</{}>", _name);
        if (!_printInline)
        {
            output << std::endl;
        }
    }

    return output.str();
}

void HtmlElement::ToString(std::stringstream& output)
{
    output << ToString();
}
} // namespace hokee