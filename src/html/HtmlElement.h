#pragma once

#include "IHtmlPrintable.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace hokee
{
class HtmlElement : public IHtmlPrintable
{
    bool _printInline{false};
    int _indent{2};
    std::string _name{};
    std::vector<std::unique_ptr<IHtmlPrintable>> _elements{};
    std::unordered_map<std::string, std::string> _attributes{};

    void SetIndent(int indent);
    void ToString(std::ostream& output) const final;

  public:
    HtmlElement() = delete;
    HtmlElement(const std::string& name);
    HtmlElement(const std::string& name, const std::string& text);
    virtual ~HtmlElement() = default;

    void AddElement(const std::string& text);
    void AddElement(std::unique_ptr<IHtmlPrintable>&& element);
    void AddAttribute(const std::string& attributeName, const std::string& attributeValue);
};

} // namespace hokee