#pragma once

#include "IPrintable.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace hokee
{
class HtmlElement : public IPrintable
{
    bool _printInline{false};
    int _indent{0};
    std::string _name{};
    std::vector<std::unique_ptr<IPrintable>> _elements{};
    std::unordered_map<std::string, std::string> _attributes{};

    void SetIndent(int indent);
    void ToString(std::ostream& output) const final;

    HtmlElement* AddElement(const std::string& name, const std::string& text = "");

  public:
    HtmlElement();
    HtmlElement(const std::string& name, const std::string& text);
    virtual ~HtmlElement() = default;

    void SetAttribute(const std::string& attributeName, const std::string& attributeValue);

    void AddText(const std::string& text);
    HtmlElement* AddBody(const std::string& text = "");
    HtmlElement* AddBold(const std::string& text = "");
    HtmlElement* AddBreak();
    HtmlElement* AddDivision(const std::string& text = "");
    HtmlElement* AddFooter(const std::string& text = "");
    HtmlElement* AddHead(const std::string& text = "");
    HtmlElement* AddHeader(const std::string& text = "");
    HtmlElement* AddHeading(int size, const std::string& text = "");
    HtmlElement* AddHyperlink(const std::string& link, const std::string& title, const std::string& text = "");
    HtmlElement* AddImage(const std::string& src, const std::string& title, int width, int height);
    HtmlElement* AddImage(const std::string& src, const std::string& title, int size);
    HtmlElement* AddLink(const std::string& text = "");
    HtmlElement* AddMain(const std::string& text = "");
    HtmlElement* AddMeta(const std::string& text = "");
    HtmlElement* AddParagraph(const std::string& text = "");
    HtmlElement* AddStyle(const std::string& text = "");
    HtmlElement* AddTable(const std::string& text = "");
    HtmlElement* AddTableCell(const std::string& text = "");
    HtmlElement* AddTableHeaderCell(const std::string& text = "");
    HtmlElement* AddTableRow(const std::string& text = "");
    HtmlElement* AddTitle(const std::string& text = "");
};

} // namespace hokee