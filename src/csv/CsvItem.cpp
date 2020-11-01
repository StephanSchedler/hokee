#include "CsvItem.h"
#include <sstream>

namespace hokee
{
std::string CsvItem::ToString()
{
    std::stringstream result;

    result << Category << " | " << PayerPayee << " | " << Description << " | "
           << Type << " | " << Date << " | " << Value;
    return result.str();
}

void CsvItem::Match(const std::shared_ptr<CsvItem>& rule)
{
    bool match = rule->Date.GetYear() < 0 || Date.ToString() == rule->Date.ToString();
    match = match && (rule->Value.ToString().empty() || Value.ToString() == rule->Value.ToString());
    match = match && Utils::ToLower(Type).find(rule->Type) != std::string::npos;
    match = match && Utils::ToLower(PayerPayee).find(rule->PayerPayee) != std::string::npos;
    match = match && Utils::ToLower(Account).find(rule->Account) != std::string::npos;
    match = match && Utils::ToLower(Description).find(rule->Description) != std::string::npos;

    if (match)
    {
        this->References.push_back(rule.get());
        rule->References.push_back(this);
        this->Category = rule->Category;
    }
}
} // namespace hokee