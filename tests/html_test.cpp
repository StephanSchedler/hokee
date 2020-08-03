#include "UserException.h"
#include "Utils.h"
#include "html/HtmlPage.h"
#include "html/HtmlTitle.h"
#include "html/HtmlText.h"
#include "html/HtmlHeading.h"
#include "html/HtmlParagraph.h"
#include "html/HtmlBreak.h"


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

        auto h2 = std::make_unique<HtmlHeading>(2,"Heading 2");
        body->AddElement(std::move(h2));
        
        auto p1 = std::make_unique<HtmlParagraph>();
        p1->AddElement("Some text");
        p1->AddElement(std::make_unique<HtmlBreak>());
        p1->AddElement("Some text 2");
        body->AddElement(std::move(p1));

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
