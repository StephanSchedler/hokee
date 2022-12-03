#include "CsvItem.h"
#include <fmt/format.h>
#include <regex>
#include <sstream>

#define MATCH(a, b) ((b).empty() || std::regex_search(Utils::ToLower(a), std::regex(Utils::ToLower(b))))

namespace hokee
{
std::string CsvItem::ToString()
{
    std::stringstream result;

    result << Category << " | " << PayerPayee << " | " << Description << " | " << Type << " | " << Date << " | "
           << Value;
    return result.str();
}

void CsvItem::Match(const std::shared_ptr<CsvItem>& rule)
{
    bool match = rule->Date.GetYear() < 0 || MATCH(Date.ToString(), rule->Date.ToString());
    match = match && (rule->Value.ToString().empty() || Value.ToString() == rule->Value.ToString());
    match = match && MATCH(Type, rule->Type);
    match = match && MATCH(PayerPayee, rule->PayerPayee);
    match = match && MATCH(Account, rule->Account);
    match = match && MATCH(Description, rule->Description);

    if ((Utils::ToLower(Value.ToString()).find(rule->Value.ToString()) != std::string::npos)
        != MATCH(Value.ToString(), rule->Value.ToString()))
        std::cout << (fmt::format("'{}','{}','{}'\n", MATCH(Value.ToString(), rule->Value.ToString()),
                                  Utils::ToLower(Value.ToString()), rule->Value.ToString()));

    if (match)
    {
        this->References.push_back(rule.get());
        rule->References.push_back(this);
        this->Category = rule->Category;
    }
}
} // namespace hokee