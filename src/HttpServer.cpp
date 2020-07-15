#include "HttpServer.h"
#include "Filesystem.h"
#include "Utils.h"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <sstream>

#include <cpp-httplib/httplib.h>
#include <fmt/format.h>

namespace hokee
{
namespace
{
std::string GetParam(const httplib::Params& params, const std::string& param, const std::string& request)
{
    auto idIter = params.find(param);
    if (idIter == params.end())
    {
        return "";
    }
    Utils::PrintError(fmt::format("Read '{}' parameter from '{}' requests", param, request));
    return idIter->second;
}

std::string GetImageContent(const std::string& name)
{
    for (const auto& entry : fs::recursive_directory_iterator(fs::current_path() / ".." / "images"))
    {
        if (fs::is_regular_file(entry.path()) && entry.path().filename().string() == name)
        {
            std::ifstream ifstream(entry.path(), std::ios::binary);
            std::stringstream sstream{};
            sstream << ifstream.rdbuf();
            return sstream.str();
        }
    }
    throw UserException(fmt::format("Could not find '{}' in '{}'", fs::path(name).filename().string(),
                                    fs::absolute(fs::current_path() / ".." / "images").string()));
}

void PrintRequest(const httplib::Request& req, const httplib::Response& res)
{
    Utils::PrintTrace("================================");
    Utils::PrintTrace(fmt::format("{} {} {}", req.method, req.version, req.path));
    std::string query;
    for (auto& x : req.params)
    {
        Utils::PrintTrace(fmt::format("{}={}", x.first, x.second));
    }
    for (auto& x : req.headers)
    {
        Utils::PrintTrace(fmt::format("{}: {}", x.first, x.second));
    }
    Utils::PrintTrace("--------------------------------");

    Utils::PrintTrace(fmt::format("{} {}", res.status, res.version));
    for (auto& x : res.headers)
    {
        Utils::PrintTrace(fmt::format("{}: {}", x.first, x.second));
    }
    // if (!res.body.empty())
    //{
    //    Utils::PrintTrace(res.body);
    //}
    Utils::PrintTrace("================================");
}

std::string GetRequestId(const httplib::Request& req)
{
    std::stringstream idStream;
    idStream << req.path;
    for (auto& p : req.params)
    {
        idStream << "?" << p.first << "=" << p.second;
    }
    return idStream.str();
}
} // namespace

bool HttpServer::SetCacheContent(const httplib::Request& req, httplib::Response& res)
{
    const std::string reqId = GetRequestId(req);
    auto i = _cache.find(reqId);
    if (i != _cache.end())
    {
        res.set_content(i->second, "text/html");
        return true;
    }
    return false;
}

void HttpServer::SetContent(const httplib::Request& req, httplib::Response& res, const std::string& content,
                            const char* content_type)
{
    const std::string reqId = GetRequestId(req);
    _cache[reqId] = content;
    res.set_content(content, content_type);
}

inline void HttpServer::HandleHtmlRequest(const httplib::Request& req, httplib::Response& res)
{
    // Check cache
    if (SetCacheContent(req, res))
    {
        return;
    }

    std::unique_lock<std::mutex> lock(_pDatabase->ReadLock, std::try_to_lock);
    if (!lock.owns_lock())
    {
        res.set_content(HtmlGenerator::GetProgressPage(_pDatabase->ProgressValue, _pDatabase->ProgressMax),
                        "text/html");
        return;
    }

    // index.html
    if (req.path == std::string("/") + HtmlGenerator::INDEX_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetSummaryPage(_pDatabase), "text/html");
        return;
    }

    // all.html
    if (req.path == std::string("/") + HtmlGenerator::ALL_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_pDatabase, "All items", _pDatabase->Data), "text/html");
        return;
    }

    // assigned.html
    if (req.path == std::string("/") + HtmlGenerator::ASSIGNED_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_pDatabase, "Assigned items", _pDatabase->Assigned),
                   "text/html");
        return;
    }

    // unassigned.html
    if (req.path == std::string("/") + HtmlGenerator::UNASSIGNED_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_pDatabase, "Unassigned items", _pDatabase->Unassigned),
                   "text/html");
        return;
    }

    // rules.html
    if (req.path == std::string("/") + HtmlGenerator::RULES_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_pDatabase, "Rules", _pDatabase->Rules), "text/html");
        return;
    }

    // issues.html
    if (req.path == std::string("/") + HtmlGenerator::ISSUES_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_pDatabase, "Issues", _pDatabase->Issues), "text/html");
        return;
    }

    // item.html
    if (req.path == std::string("/") + HtmlGenerator::ITEM_HTML)
    {
        const std::string idStr = GetParam(req.params, "id", HtmlGenerator::ITEM_HTML);
        if (idStr.empty())
        {
            res.status = 404;
            std::string errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                                   HtmlGenerator::ITEM_HTML, "id");
            res.set_content(HtmlGenerator::GetErrorPage(res.status, errorMessage), "text/html");
            return;
        }
        const int id = std::stoi(idStr);
        SetContent(req, res, HtmlGenerator::GetItemPage(_pDatabase, id), "text/html");
        return;
    }

    // items.html
    if (req.path == std::string("/") + HtmlGenerator::ITEMS_HTML)
    {
        const std::string yearStr = GetParam(req.params, "year", HtmlGenerator::ITEMS_HTML);
        if (yearStr.empty())
        {
            res.status = 404;
            std::string errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                                   HtmlGenerator::ITEM_HTML, "year");
            res.set_content(HtmlGenerator::GetErrorPage(res.status, errorMessage), "text/html");
            return;
        }
        const int year = std::stoi(yearStr);
        std::string monthStr = GetParam(req.params, "month", HtmlGenerator::ITEMS_HTML);
        if (monthStr.empty())
        {
            monthStr = "0";
        }
        const int month = std::stoi(monthStr);
        const std::string cat = GetParam(req.params, "category", HtmlGenerator::ITEMS_HTML);

        CsvTable data{};
        for (auto& item : _pDatabase->Data)
        {
            if (year == item->Date.GetYear() && (month == 0 || month == item->Date.GetMonth())
                && (cat == "" || cat == item->Category))
            {
                data.push_back(item);
            }
        }

        std::string name = fmt::format("{}-{}: {}", month, year, cat);
        if (month == 0)
        {
            name = fmt::format("{}: {}", year, cat);
        }
        SetContent(req, res, HtmlGenerator::GetTablePage(_pDatabase, name, data), "text/html");
        return;
    }

    // default
    res.status = 404;
    res.set_content(HtmlGenerator::GetErrorPage(res.status, httplib::detail::status_message(res.status)),
                    "text/html");
}

HttpServer::HttpServer(CsvDatabase* pDatabase)
    : _server{std::make_unique<httplib::Server>()}
{
    if (!_server->is_valid())
    {
        throw UserException("Could not initialize HttpServer");
    }
    HttpServer::_pDatabase = pDatabase;

    // Get root
    _server->Get("/", [](const httplib::Request& /*req*/, httplib::Response& res) {
        res.set_redirect((std::string("/") + HtmlGenerator::INDEX_HTML).c_str());
    });

    // Get Html page
    _server->Get("/.*\\.html",
                 [&](const httplib::Request& req, httplib::Response& res) { HandleHtmlRequest(req, res); });

    // Get Images
    _server->Get("/(.*\\.png)", [&](const httplib::Request& req, httplib::Response& res) {
        if (!SetCacheContent(req, res))
        {
            std::string name = req.matches[1];
            SetContent(req, res, GetImageContent(name), "image/png");
        }
    });
    _server->Get("/(.*\\.ico)", [&](const httplib::Request& req, httplib::Response& res) {
        if (!SetCacheContent(req, res))
        {
            std::string name = req.matches[1];
            SetContent(req, res, GetImageContent(name), "image/x-icon");
        }
    });

    // Exit
    _server->Get((std::string("/") + HtmlGenerator::EXIT_CMD).c_str(),
                 [&](const httplib::Request& /*req*/, httplib::Response& /*res*/) {
                     Utils::PrintTrace("Received exit request. Shutdown application...");
                     _server->stop();
                 });

    // Set Error Handler
    _server->set_error_handler([&](const httplib::Request& /*req*/, httplib::Response& res) {
        res.set_content(HtmlGenerator::GetErrorPage(res.status, httplib::detail::status_message(res.status)),
                        "text/html");
    });

    // Set Logger
    _server->set_logger([](const httplib::Request& req, const httplib::Response& res) { PrintRequest(req, res); });
}

HttpServer::~HttpServer()
{
    _server->stop();
    _server.reset();
}

void HttpServer::Run()
{
    _server->listen("localhost", 80);
}

} // namespace hokee