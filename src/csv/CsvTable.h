#pragma once

#include "csv/CsvItem.h"

namespace hokee
{
class CsvTable : public std::vector<CsvRowShared>
{
    std::vector<std::string> _header{};

  public:
    inline const std::vector<std::string>& GetCsvHeader() const
    {
        return _header;
    };

    inline void SetCsvHeader(std::vector<std::string>&& header)
    {
        _header = std::move(header);
    }

    int DeleteItem(int id);
    int NextItem(int id) const;
    int PrevItem(int id) const;
    bool HasItem(int id) const;
};
} // namespace hokee