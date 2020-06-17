#pragma once

#include "csv/CsvDate.h"
#include "Utils.h"

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
    std::string Value = {};
    std::string Category = "???";
    std::vector<std::string> Issues = {};
    std::vector<CsvItem*> References = {};
    fs::path File = {};
    int Line = -1;
    int Id = -1;

    bool operator==(const CsvItem& ref) const
    {
        return Date == ref.Date && Type == ref.Type && PayerPayee == ref.PayerPayee &&
               Account == ref.Account && Description == ref.Description && Value == ref.Value;
    }
    void Match(const std::shared_ptr<CsvItem>& rule);

    std::string ToString();
};

typedef std::shared_ptr<CsvItem> CsvRowShared;
} // namespace hokee