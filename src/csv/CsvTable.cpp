#include "CsvTable.h"

namespace hokee
{
int CsvTable::DeleteItem(int id)
{
    auto it = this->begin();
    while (it != this->end())
    {
        if ((*it) && (*it)->Id == id)
        {
            it = this->erase(it);
            if (it != this->end() && (*it))
            {
                return (*it)->Id;
            }
            return this->PrevItem(id);
        }
        else
        {
            ++it;
        }
    }
    return -1;
}

int CsvTable::NextItem(int id) const
{
    int next = -1;
    for (auto& i : *this)
    {
        if (!i)
        {
            continue;
        }
        if (i->Id > id && (next == -1 || i->Id < next))
        {
            next = i->Id;
        }
    }
    return next;
}

int CsvTable::PrevItem(int id) const
{
    int prev = -1;
    for (auto& i : *this)
    {
        if (!i)
        {
            continue;
        }
        if (i->Id < id && (prev == -1 || i->Id > prev))
        {
            prev = i->Id;
        }
    }
    return prev;
}

bool CsvTable::HasItem(int id) const
{
    for (auto& i : *this)
    {
        if (!i)
        {
            continue;
        }
        if (i->Id == id)
        {
            return true;
        }
    }
    return false;
}
} // namespace hokee