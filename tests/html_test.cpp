#include "UserException.h"
#include "Utils.h"
#include "html/HtmlElement.h"
#include "Filesystem.h"

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
        HtmlElement html;
        auto head = html.AddHead();
        head->AddTitle("Title");

        auto meta = head->AddMeta();
        meta->SetAttribute("name", "html_test");
        meta->SetAttribute("content", "unit-test");
        meta->SetAttribute("charset", "UTF-8");

        auto link = head->AddLink();
        link->SetAttribute("rel", "icon");
        link->SetAttribute("type", "image/jpg");
        link->SetAttribute("sizes", "32x32");
        link->SetAttribute("href", "https://www.fillmurray.com/32/32");

        auto body = html.AddBody();
        auto header = body->AddHeader();
        header->AddHeading(2, "Heading 2");

        auto main = body->AddMain();
        auto div = main->AddDivision();
        auto table = div->AddTable();
        auto row = table->AddTableRow();
        row->AddTableHeaderCell("Head 1");
        row->AddTableHeaderCell("Head 2");
        row = table->AddTableRow();
        auto cell = row->AddTableCell();
        cell->AddText("AAA BBB");
        cell->AddBreak();
        cell->AddText("CCC <escaped>");
        cell->AddBreak();
        cell->AddText("DDD \"EEE\"");
        cell->AddBreak();
        cell->AddText("GGG.");
        cell = row->AddTableCell();
        cell->AddImage("https://www.fillmurray.com/200/250", "fillmurray", 200, 250);

        auto footer = body->AddFooter();
        auto p = footer->AddParagraph();
        p->AddBold("BOLD: ");
        p->AddHyperlink("https://www.fillmurray.com", "Link to fillmurray.com", "www.fillmurray.com");
        
        fs::path htmlPath = Utils::GetTempDir() / "index.html";
        std::ofstream ofStream;
        ofStream.open(htmlPath, std::ios::binary);
        ofStream << html;
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
