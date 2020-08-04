#include "UserException.h"
#include "Utils.h"
#include "html/HtmlPage.h"

#include <filesystem>
#include <fmt/format.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <thread>

using namespace hokee;

int main(int /*unused*/, const char** /*unused*/)
{
    std::set_terminate(Utils::TerminationHandler);

    bool success = true;
    try
    {
        auto head = std::make_unique<HtmlHead>();
        head->AddElement(std::make_unique<HtmlTitle>("Title"));

        auto meta = std::make_unique<HtmlMeta>();
        meta->AddAttribute("name", "html_test");
        meta->AddAttribute("content", "unit-test");
        meta->AddAttribute("charset", "UTF-8");
        head->AddElement(std::move(meta));

        auto link = std::make_unique<HtmlLink>();
        link->AddAttribute("rel", "icon");
        link->AddAttribute("type", "image/jpg");
        link->AddAttribute("sizes", "32x32");
        link->AddAttribute("href", "https://www.fillmurray.com/32/32");
        head->AddElement(std::move(link));

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
        p->AddElement(std::make_unique<HtmlHyperlink>("https://www.fillmurray.com", "Link to fillmurray.com",
                                                      "www.fillmurray.com"));
        footer->AddElement(std::move(p));

        body->AddElement(std::move(footer));

        HtmlPage htmlPage(std::move(head), std::move(body));

        fs::path htmlPath = Utils::GetTempDir() / "index.html";
        std::ofstream ofStream;
        ofStream.open(htmlPath, std::ios::binary);
        ofStream << htmlPage;
        ofStream.close();

        fs::path referencePath = fs::path("..") / "test_data" / "html_test" / "index.html";
        success = Utils::CompareFiles(htmlPath, referencePath);

        if (success)
        {
            Utils::PrintInfo("TEST PASSED");
        }
        else
        {
            fs::path testDataPath = fs::current_path() / ".." / "tests" / "test_data" / "index.html";
            Utils::PrintInfo("Update test data:");
            Utils::PrintInfo(fmt::format("  copy /Y \"{}\" \"{}\"", htmlPath.make_preferred().string(),
                                         testDataPath.make_preferred().string()));
            Utils::PrintInfo(fmt::format("  cp -f \"{}\" \"{}\"", htmlPath.string(), testDataPath.string()));

#ifdef _MSC_VER
            Utils::RunAsync(fmt::format("start {}", htmlPath.string()));
            std::this_thread::sleep_for(std::chrono::seconds(1));
#endif

            Utils::PrintInfo("TEST FAILED");
        }
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
