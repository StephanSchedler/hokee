#include "IHtmlPrintable.h"

#include <memory>

namespace hokee
{
std::ostream& operator<<(std::ostream& os, const IHtmlPrintable& obj)
{
    obj.ToString(os);
    return os;
}

} // namespace hokee