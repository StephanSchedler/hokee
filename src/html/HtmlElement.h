#pragma once

#include "IPrintable.h"

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>


namespace hokee
{
class HtmlElement : public IPrintable
{
    bool _printInline{false};
    int _indent{0};
    std::string _name{};
    std::vector<std::unique_ptr<IPrintable>> _elements{};
    std::vector<std::pair<std::string, std::string>> _attributes{};

    void SetIndent(int indent);
    void ToString(std::ostream& output) const final;

    HtmlElement* AddElement(const std::string& name, const std::string& text = "");

  public:
    HtmlElement();
    HtmlElement(const std::string& name, const std::string& text);
    virtual ~HtmlElement() = default;

    void AddBreak();
    void AddText(const std::string& text);
    void AddProgress(size_t value, size_t max);
    void AddImage(const std::string& src, const std::string& title, int width, int height, bool blink = false);
    void AddImage(const std::string& src, const std::string& title, int size, bool blink = false);
    void AddHyperlinkImage(const std::string& link, const std::string& title, const std::string& src, int width,
                           int height);
    void AddHyperlinkImage(const std::string& link, const std::string& title, const std::string& src, int size);

    HtmlElement* AddBody(const std::string& text = "");
    HtmlElement* AddBold(const std::string& text = "");
    HtmlElement* AddDivision(const std::string& text = "");
    HtmlElement* AddFooter(const std::string& text = "");
    HtmlElement* AddForm(const std::string& text = "");
    HtmlElement* AddHead(const std::string& text = "");
    HtmlElement* AddHeader(const std::string& text = "");
    HtmlElement* AddHeading(int size, const std::string& text = "");
    HtmlElement* AddHyperlink(const std::string& link, const std::string& title, const std::string& text = "");
    HtmlElement* AddInput(const std::string& text = "");
    HtmlElement* AddLink(const std::string& text = "");
    HtmlElement* AddMain(const std::string& text = "");
    HtmlElement* AddMeta(const std::string& text = "");
    HtmlElement* AddParagraph(const std::string& text = "");
    HtmlElement* AddStyle(const std::string& text = "");
    HtmlElement* AddTable(const std::string& text = "");
    HtmlElement* AddTableCell(const std::string& text = "");
    HtmlElement* AddTableHeaderCell(const std::string& text = "");
    HtmlElement* AddTextarea(const std::string& text = "");
    HtmlElement* AddTableRow(const std::string& text = "");
    HtmlElement* AddTitle(const std::string& text = "");

    void SetAttribute(const std::string& attributeName, const std::string& attributeValue);
    std::string ToString() const;
};

} // namespace hokee