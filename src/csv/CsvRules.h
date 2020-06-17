#pragma once

#include "InternalException.h"
#include "csv/CsvTable.h"

namespace hokee
{
class CsvRules final : public CsvTable
{
  public:
    inline const std::vector<std::string> GetCategories()
    {
        const auto rulesFormat = Utils::GetCsvFormat("Rules");
        const auto rulesHeader = this->GetCsvHeader();
        if (rulesHeader.size() != rulesFormat.IgnoreLines)
        {
            throw InternalException(__FILE__, __LINE__, "Internal header of rule set does not match expected format!");
        }
        
        return Utils::SplitLine(rulesHeader[1], rulesFormat);
    };
};
} // namespace hokee