#include "UserException.h"
#include "Utils.h"
#include "html/HtmlPage.h"

#include <fmt/format.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>

using namespace hokee;

int main(int /*unused*/, const char** /*unused*/)
{
    std::set_terminate(Utils::TerminationHandler);

    bool success = true;
    try
    {
        fs::path htmlPath = Utils::GetTempDir() / "index.html";
        std::ofstream htmlStream;
        htmlStream.open(htmlPath, std::ios::binary);

        auto head = std::make_unique<HtmlHead>();
        head->AddElement(std::make_unique<HtmlTitle>("Title"));
        auto body = std::make_unique<HtmlBody>();

        auto header = std::make_unique<HtmlHeader>();

        auto h2 = std::make_unique<HtmlHeading>(2, "Heading 2");
        header->AddElement(std::move(h2));

        body->AddElement(std::move(header));

        auto main = std::make_unique<HtmlMain>();

        auto div = std::make_unique<HtmlDivision>();

        auto table = std::make_unique<HtmlTable>();

        auto tableHeader = std::make_unique<HtmlTableRow>();

        auto tableHead1 = std::make_unique<HtmlTableHeader>();
        tableHead1->AddElement("Head 1");
        tableHeader->AddElement(std::move(tableHead1));

        auto tableHead2 = std::make_unique<HtmlTableHeader>();
        tableHead2->AddElement("Head 2");
        tableHeader->AddElement(std::move(tableHead2));

        table->AddElement(std::move(tableHeader));

        auto tableRow1 = std::make_unique<HtmlTableRow>();

        auto tableCell1 = std::make_unique<HtmlTableCell>();

        tableCell1->AddElement("AAA BBB");
        tableCell1->AddElement(std::make_unique<HtmlBreak>());
        tableCell1->AddElement("CCC <escaped>");
        tableCell1->AddElement(std::make_unique<HtmlBreak>());
        tableCell1->AddElement("DDD \"EEE\"");
        tableCell1->AddElement(std::make_unique<HtmlBreak>());
        tableCell1->AddElement("GGG.");

        tableRow1->AddElement(std::move(tableCell1));

        auto tableCell2 = std::make_unique<HtmlTableCell>();

        tableCell2->AddElement(
            std::make_unique<HtmlImage>("https://www.fillmurray.com/200/250", "fillmurray", 200, 250));

        tableRow1->AddElement(std::move(tableCell2));

        table->AddElement(std::move(tableRow1));

        div->AddElement(std::move(table));

        main->AddElement(std::move(div));

        body->AddElement(std::move(main));

        auto footer = std::make_unique<HtmlFooter>();

        auto p = std::make_unique<HtmlParagraph>();
        p->AddElement(std::make_unique<HtmlBold>("BOLD: "));
        p->AddElement(std::make_unique<HtmlLink>("https://www.fillmurray.com", "Link to fillmurray.com",
                                                 "www.fillmurray.com"));
        footer->AddElement(std::move(p));

        body->AddElement(std::move(footer));

        HtmlPage htmlPage(std::move(head), std::move(body));
        htmlStream << htmlPage;

        htmlStream.close();

        Utils::RunSync(fmt::format("start {}", htmlPath.string()));
    }
    catch (const UserException& e)
    {
        Utils::TerminationHandler(e);
    }
    catch (const std::exception& e)
    {
        Utils::TerminationHandler(e);
    }
    catch (...)
    {
        Utils::TerminationHandler();
    }

    return success ? 0 : -1;
}
