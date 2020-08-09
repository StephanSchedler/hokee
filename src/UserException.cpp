#include "UserException.h"
#include "html/HtmlGenerator.h"

#include <fmt/format.h>
#include <sstream>
#include <string>

namespace hokee
{
namespace
{
std::string GenerateMessage(const std::string& msg, const fs::path& file, int line)
{
    HtmlElement div("div", msg);
    if (!file.empty())
    {
        div.AddText(" ");
        div.AddBreak();
        div.AddText(file.string());
        if (line > 0)
        {
            div.AddText(fmt::format(":{}", line));
        }
        HtmlGenerator::AddEditorHyperlink(&div, file);
    }
    return div.ToString();
}
} // namespace

UserException::UserException(const std::string& msg, const fs::path& file, int line)
    : std::runtime_error(GenerateMessage(msg, file, line).c_str())
    , _file{file}
    , _line{line}
{
}

const fs::path UserException::GetFile() const
{
    return _file;
}

int UserException::GetLine() const
{
    return _line;
}
} // namespace hokee
