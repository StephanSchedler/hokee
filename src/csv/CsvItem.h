#pragma once

#include "CsvDate.h"
#include "CsvValue.h"
#include "../Utils.h"

#include <memory>
#include <string>
#include <vector>

namespace hokee
{

struct CsvItem
{
    CsvDate Date = {};
    std::string Type = {};
    std::string PayerPayee = {};
    std::string Payer = {};
    std::string Payee = {};
    std::string Account = {};
    std::string Description = {};
    CsvValue Value = {};
    std::string Category = {};
    std::vector<std::string> Issues = {};
    std::vector<CsvItem*> References = {};
    fs::path File = {};
    int Line = -1;
    int Id = -1;

    bool operator==(const CsvItem& ref) const
    {
        return Date.ToString() == ref.Date.ToString() && Type == ref.Type && PayerPayee == ref.PayerPayee &&
               Account == ref.Account && Description == ref.Description && Value.ToString() == ref.Value.ToString();
    }
    void Match(const std::shared_ptr<CsvItem>& rule);

    std::string ToString();
};

typedef std::shared_ptr<CsvItem> CsvRowShared;
} // namespace hokee