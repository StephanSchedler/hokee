#include "UserException.h"
#include "Utils.h"
#include "html/HtmlPage.h"
#include "html/HtmlTitle.h"
#include "html/HtmlText.h"
#include "html/HtmlHeading.h"
#include "html/HtmlLink.h"
#include "html/HtmlParagraph.h"
#include "html/HtmlDivision.h"
#include "html/HtmlHeader.h"
#include "html/HtmlMain.h"
#include "html/HtmlBold.h"
#include "html/HtmlFooter.h"
#include "html/HtmlImage.h"
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

        auto header = std::make_unique<HtmlHeader>();

        auto h2 = std::make_unique<HtmlHeading>(2,"Heading 2");
        header->AddElement(std::move(h2));
        
        body->AddElement(std::move(header));
        
        auto main = std::make_unique<HtmlMain>();
        
        auto p = std::make_unique<HtmlParagraph>();
        p->AddElement("AAA BBB");
        p->AddElement(std::make_unique<HtmlBreak>());
        p->AddElement("CCC <escaped> DDD \"EEE\" GGG.");
        main->AddElement(std::move(p));
  
        auto img = std::make_unique<HtmlImage>("https://www.fillmurray.com/200/100","fillmurray", 200, 100);
        main->AddElement(std::move(img));
      
        body->AddElement(std::move(main));

        auto footer = std::make_unique<HtmlFooter>();

        auto div = std::make_unique<HtmlDivision>();
        div->AddElement(std::make_unique<HtmlBold>("BOLD: "));
        div->AddElement(std::make_unique<HtmlLink>("https://www.fillmurray.com", "Link to fillmurray.com", "www.fillmurray.com"));
        footer->AddElement(std::move(div));
        
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
