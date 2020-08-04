#include "IPrintable.h"

#include <memory>

namespace hokee
{
std::ostream& operator<<(std::ostream& os, const IPrintable& obj)
{
    obj.ToString(os);
    return os;
}

} // namespace hokee