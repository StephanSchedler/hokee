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

    int NextItem(int id) const
    {
        int next = -1;
        for (auto& i : *this)
        {
            if (i->Id > id && (next == -1 || i->Id < next ))
            {
                next = i->Id;
            }
        }
        return next;
    }

    int PrevItem(int id) const
    {
        int prev = -1;
        for (auto& i : *this)
        {
            if (i->Id < id && (prev == -1 || i->Id > prev ))
            {
                prev = i->Id;
            }
        }
        return prev;
    }

    bool HasItem(int id) const
    {
        for (auto& i : *this)
        {
            if (i->Id == id)
            {
                return true;
            }
        }
        return false;
    }
};
} // namespace hokee