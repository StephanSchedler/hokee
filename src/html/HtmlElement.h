#pragma once

#include "IHtmlPrintable.h"

#include <memory>
#include <unordered_map>
#include <vector>


namespace hokee
{
class HtmlElement : IHtmlPrintable
{
    bool _printInline{false};
    int _indent{0};
    std::string _name{};
    std::vector<std::unique_ptr<IHtmlPrintable>> _elements{};
    std::unordered_map<std::string, std::string> _attributes{};

    void SetIndent(int indent);

  public:
    HtmlElement() = delete;
    HtmlElement(const std::string& name, bool printInline);
    virtual ~HtmlElement() = default;

    void AddElement(std::unique_ptr<IHtmlPrintable>&& element);
    void SetElements(std::vector<std::unique_ptr<IHtmlPrintable>>&& elements);
    void AddAttribute(const std::string& attributeName, const std::string& attributeValue);

    std::string ToString() override;
    void ToString(std::stringstream& output) override;
};

} // namespace hokee