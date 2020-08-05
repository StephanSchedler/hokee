#include "HtmlElement.h"
#include "html/HtmlText.h"

#include <fmt/format.h>

namespace hokee
{
HtmlElement::HtmlElement()
    : HtmlElement("html", "")
{
    SetAttribute("lang", "en");
}

HtmlElement::HtmlElement(const std::string& name, const std::string& text)
    : _printInline{!text.empty()}
    , _name{name}
{
    if (!text.empty())
    {
        _elements.push_back(std::make_unique<HtmlText>(text));
    }
}

void HtmlElement::SetIndent(int indent)
{
    _indent = indent;
}

void HtmlElement::SetAttribute(const std::string& attributeName, const std::string& attributeValue)
{
    _attributes.emplace_back(attributeName, fmt::format("\"{}\"", attributeValue));
}

void HtmlElement::ToString(std::ostream& output) const
{
    if (_name == "html")
    {
        output << "<!DOCTYPE html>" << std::endl;
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

HtmlElement* HtmlElement::AddElement(const std::string& name, const std::string& text)
{
    auto element = std::make_unique<HtmlElement>(name, text);
    element->SetIndent(_indent + 2);
    auto ptr = element.get();
    _elements.push_back(std::move(element));
    return ptr;
}

void HtmlElement::AddText(const std::string& text)
{
    _elements.push_back(std::make_unique<HtmlText>(text));
}

HtmlElement* HtmlElement::AddBody(const std::string& text)
{
    return AddElement("body", text);
}

HtmlElement* HtmlElement::AddBold(const std::string& text)
{
    return AddElement("b", text);
}

HtmlElement* HtmlElement::AddBreak()
{
    return AddElement("br");
}

HtmlElement* HtmlElement::AddDivision(const std::string& text)
{
    return AddElement("div", text);
}

HtmlElement* HtmlElement::AddFooter(const std::string& text)
{
    return AddElement("footer", text);
}

HtmlElement* HtmlElement::AddHead(const std::string& text)
{
    return AddElement("head", text);
}

HtmlElement* HtmlElement::AddHeader(const std::string& text)
{
    return AddElement("header", text);
}

HtmlElement* HtmlElement::AddHeading(int size, const std::string& text)
{
    return AddElement(fmt::format("h{}", size), text);
}

HtmlElement* HtmlElement::AddHyperlink(const std::string& link, const std::string& title, const std::string& text)
{
    auto element = AddElement("a", text);
    element->SetAttribute("href", link);
    element->SetAttribute("title", title);
    return element;
}

HtmlElement* HtmlElement::AddImage(const std::string& src, const std::string& title, int width, int height)
{
    auto element = AddElement("img");
    element->SetAttribute("src", src);
    element->SetAttribute("title", title);
    element->SetAttribute("width", fmt::format("{}", width));
    element->SetAttribute("height", fmt::format("{}", height));
    return element;
}

HtmlElement* HtmlElement::AddImage(const std::string& src, const std::string& title, int size)
{
    return AddImage(src, title, size, size);
}

HtmlElement* HtmlElement::AddLink(const std::string& text)
{
    return AddElement("link", text);
}

HtmlElement* HtmlElement::AddMain(const std::string& text)
{
    return AddElement("main", text);
}

HtmlElement* HtmlElement::AddMeta(const std::string& text)
{
    return AddElement("meta", text);
}

HtmlElement* HtmlElement::AddParagraph(const std::string& text)
{
    return AddElement("p", text);
}

HtmlElement* HtmlElement::AddStyle(const std::string& text)
{
    return AddElement("style", text);
}

HtmlElement* HtmlElement::AddTable(const std::string& text)
{
    return AddElement("table", text);
}

HtmlElement* HtmlElement::AddTableCell(const std::string& text)
{
    return AddElement("td", text);
}

HtmlElement* HtmlElement::AddTableHeaderCell(const std::string& text)
{
    return AddElement("th", text);
}

HtmlElement* HtmlElement::AddTableRow(const std::string& text)
{
    return AddElement("tr", text);
}

HtmlElement* HtmlElement::AddTitle(const std::string& text)
{
    return AddElement("title", text);
}
} // namespace hokee