#pragma once

#include "Utils.h"
#include "csv/CsvDatabase.h"

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>

namespace hokee
{
class HtmlElement
{
  bool _printInline{false};
  int _indent{0};
  std::string _name{};
  std::vector<std::unique_ptr<HtmlElement>> _elements{};
  std::unordered_map<std::string,std::string> _attributes{};
  
  public:
    HtmlElement() = delete;
    HtmlElement(const std::string& name, bool printInline);
    ~HtmlElement() = default;

    HtmlElement(const HtmlElement&) = delete;
    HtmlElement& operator=(const HtmlElement&) = delete;
    HtmlElement(HtmlElement&&) = delete;
    HtmlElement& operator=(HtmlElement&&) = delete;

    void SetIndent(int indent);
    void AddElement(std::unique_ptr<HtmlElement>&& element);
    void AddAttribute(const std::string& attributeName, const std::string& attributeValue);
    
    std::string ToString();
    void ToString(std::stringstream& output);
};

} // namespace hokee