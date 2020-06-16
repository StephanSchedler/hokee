#pragma once

#include "CsvItem.h"

namespace hokeeboo
{
class CsvTable : public std::vector<CsvRowShared>
{
  public:
    std::vector<std::string> Header{};
};
} // namespace hokeeboo