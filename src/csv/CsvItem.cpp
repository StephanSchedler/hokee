#include "CsvItem.h"
#include <fmt/format.h>
#include <regex>
#include <sstream>

namespace hokee
{
void CsvItem::ToLower()
{
        this->Account = Utils::ToLower(this->Account);
        this->Category = Utils::ToLower(this->Category);
        this->Description = Utils::ToLower(this->Description);
        this->Payer = Utils::ToLower(this->Payer);
        this->Payee = Utils::ToLower(this->Payee);
        this->PayerPayee = Utils::ToLower(this->PayerPayee);
        this->Type = Utils::ToLower(this->Type);
}

void CsvItem::UpdateRegex()
{
        this->AccountRegex = std::regex(this->Account);
        this->DescriptionRegex = std::regex(this->Description);
        this->PayerPayeeRegex = std::regex(this->PayerPayee);
        this->TypeRegex = std::regex(this->Type);
}

std::string CsvItem::ToString()
{
    std::stringstream result;

    result << Category << " | " << PayerPayee << " | " << Description << " | " << Type << " | " << Date << " | "
           << Value;
    return result.str();
}

void CsvItem::Match(const std::shared_ptr<CsvItem>& rule)
{
    bool match = true;
    match = match && (rule->PayerPayee.empty() || std::regex_search(PayerPayee, rule->PayerPayeeRegex));
    match = match && (rule->Description.empty() || std::regex_search(Description, rule->DescriptionRegex));
    match = match && (rule->Date.GetYear() < 0 || Date.ToString() == rule->Date.ToString());
    match = match && (rule->Type.empty() || std::regex_search(Type, rule->TypeRegex));
    match = match && (rule->Account.empty() || std::regex_search(Account, rule->AccountRegex));
    match = match && (rule->Value.ToString().empty() || Value.ToString() == rule->Value.ToString());

    if (match)
    {
        this->References.push_back(rule.get());
        rule->References.push_back(this);
        this->Category = rule->Category;
    }
}
} // namespace hokee