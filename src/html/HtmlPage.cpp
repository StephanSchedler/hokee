#include "HtmlPage.h"

#include <fmt/format.h>
#include <sstream>

namespace hokee
{
HtmlPage::HtmlPage(std::unique_ptr<HtmlHead> head, std::unique_ptr<HtmlBody> body)
    : _head{std::move(head)}
    , _body{std::move(body)}
{
}

void HtmlPage::ToString(std::ostream& output) const
{
    output << "<!DOCTYPE html>" << std::endl;
    output << "<html>" << std::endl;
    output << *_head;
    output << *_body;
    output << "</html>" << std::endl;
}
} // namespace hokee