#pragma once

#include "InternalException.h"
#include "csv/CsvTable.h"
#include "csv/CsvFormat.h"

namespace hokee
{
class CsvRules final : public CsvTable
{
  public:
    static CsvFormat GetFormat();
    std::vector<std::string> GetCategories();
};
} // namespace hokee