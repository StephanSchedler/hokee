#include "HttpServer.h"
#include "Filesystem.h"
#include "InternalException.h"
#include "Settings.h"
#include "csv/CsvWriter.h"
#include "Utils.h"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
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
    Utils::PrintTrace(fmt::format("Read '{}'=='{}' parameter from '{}' requests", param, idIter->second, request));
    return idIter->second;
}

std::string GetImageContent(const std::string& name)
{
    for (const auto& entry : fs::recursive_directory_iterator(fs::current_path() / ".." / "html" / "images"))
    {
        if (fs::is_regular_file(entry.path()) && entry.path().filename().string() == name)
        {
            return Utils::ReadFileContent(entry.path());
        }
    }
    throw UserException(fmt::format("Could not find '{}'", fs::path(name).filename().string()),
                        fs::absolute(fs::current_path() / ".." / "html" / "images"));
}

std::string GetUrl(const httplib::Request& req)
{
    std::stringstream idStream;
    idStream << req.path;
    size_t paramCount = 0;
    for (auto& p : req.params)
    {
        if (0 == paramCount++)
        {
            idStream << "?" << p.first << "=" << p.second;
        }
        else
        {
            idStream << "&" << p.first << "=" << p.second;
        }
    }
    return idStream.str();
}

void PrintRequest(const httplib::Request& req, const httplib::Response& res)
{
    std::stringstream reqStream{};
    reqStream << fmt::format("HTTP-REQUEST:  {} {} {}", req.method, req.version, GetUrl(req));
    // for (auto& x : req.headers)
    //{
    //    reqStream << fmt::format(" {}:{}", x.first, x.second);
    //}
    Utils::PrintTrace(reqStream.str());

    std::stringstream resStream{};
    resStream << fmt::format("HTTP-RESPONSE: {} {}", res.status, res.version);
    for (auto& x : res.headers)
    {
        resStream << fmt::format(" {}:{}", x.first, x.second);
    }
    Utils::PrintTrace(resStream.str());
}

} // namespace

bool HttpServer::TrySetContentFromCache(const httplib::Request& req, httplib::Response& res)
{
    std::scoped_lock lock(_cacheMutex);
    const std::string url = GetUrl(req);
    auto i = _cache.find(url);
    if (i != _cache.end())
    {
        res.set_content(i->second.first, i->second.second.c_str());
        if (std::string(i->second.second) == CONTENT_TYPE_HTML)
        {
            _lastUrl = i->first;
        }
        return true;
    }
    return false;
}

void HttpServer::SetContent(const httplib::Request& req, httplib::Response& res, const std::string& content,
                            const char* content_type)
{
    if (std::string(content_type) == CONTENT_TYPE_HTML)
    {
        _lastUrl = GetUrl(req);
    }
    res.set_content(content, content_type);
}

void HttpServer::SetContentAndSetCache(const httplib::Request& req, httplib::Response& res,
                                       const std::string& content, const char* content_type)
{
    std::scoped_lock lock(_cacheMutex);
    const std::string url = GetUrl(req);
    _cache[url] = std::pair<std::string, std::string>(content, content_type);
    if (std::string(content_type) == CONTENT_TYPE_HTML)
    {
        _lastUrl = url;
    }
    res.set_content(content, content_type);
}

inline void HttpServer::HandleHtmlRequest(const httplib::Request& req, httplib::Response& res)
{
    // Check Error
    if (_errorStatus != 200)
    {
        res.status = _errorStatus;
        return;
    }

    // support.html
    if (req.path == std::string("/") + HtmlGenerator::SUPPORT_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetSupportPage(_database, _ruleSetFile, _inputDirectory),
                   CONTENT_TYPE_HTML);
        return;
    }

    // settings.html
    if (req.path == std::string("/") + HtmlGenerator::SETTINGS_HTML)
    {
        Settings config(fs::absolute(_configFile));
        bool save = false;
        std::string value = GetParam(req.params, "InputDirectory", HtmlGenerator::SETTINGS_HTML);
        if (!value.empty())
        {
            config.SetInputDirectory(value);
            save = true;
        }
        value = GetParam(req.params, "Browser", HtmlGenerator::SETTINGS_HTML);
        if (!value.empty())
        {
            config.SetBrowser(value);
            save = true;
        }
        value = GetParam(req.params, "RuleSetFile", HtmlGenerator::SETTINGS_HTML);
        if (!value.empty())
        {
            config.SetRuleSetFile(value);
            save = true;
        }
        value = GetParam(req.params, "Explorer", HtmlGenerator::SETTINGS_HTML);
        if (!value.empty())
        {
            config.SetExplorer(value);
            save = true;
        }
        value = GetParam(req.params, "Editor", HtmlGenerator::SETTINGS_HTML);
        if (!value.empty())
        {
            config.SetEditor(value);
            save = true;
        }
        if (save)
        {
            config.Save(fs::absolute(_configFile));
            res.set_redirect(HtmlGenerator::SETTINGS_HTML);
        }
        else
        {
            SetContent(req, res, HtmlGenerator::GetSettingsPage(_database, fs::absolute(_configFile)),
                       CONTENT_TYPE_HTML);
        }
        return;
    }

    // Check for empty input folder
    auto inputFolderCount = std::distance(fs::directory_iterator(_inputDirectory), fs::directory_iterator{});
    if (inputFolderCount == 0)
    {
        res.set_content(HtmlGenerator::GetEmptyInputPage(), CONTENT_TYPE_HTML);
        return;
    }

    // Check Cache
    if (TrySetContentFromCache(req, res))
    {
        return;
    }

    // Progress Page
    std::unique_lock<std::timed_mutex> lock(_databaseMutex, std::try_to_lock);
    if (!lock.owns_lock())
    {
        res.set_content(HtmlGenerator::GetProgressPage(_database.ProgressValue, _database.ProgressMax),
                        CONTENT_TYPE_HTML);
        return;
    }

    // index.html
    if (req.path == std::string("/") + HtmlGenerator::INDEX_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetSummaryPage(_database), CONTENT_TYPE_HTML);
        return;
    }

    // all.html
    if (req.path == std::string("/") + HtmlGenerator::ALL_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "All items", _database.Data),
                   CONTENT_TYPE_HTML);
        return;
    }

    // assigned.html
    if (req.path == std::string("/") + HtmlGenerator::ASSIGNED_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "Assigned items", _database.Assigned),
                   CONTENT_TYPE_HTML);
        return;
    }

    // unassigned.html
    if (req.path == std::string("/") + HtmlGenerator::UNASSIGNED_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "Unassigned items", _database.Unassigned),
                   CONTENT_TYPE_HTML);
        return;
    }

    // help.html
    if (req.path == std::string("/") + HtmlGenerator::HELP_HTML)
    {
        SetContentAndSetCache(req, res, HtmlGenerator::GetHelpPage(_database), CONTENT_TYPE_HTML);
        return;
    }

    // rules.html
    if (req.path == std::string("/") + HtmlGenerator::RULES_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "Rules", _database.Rules), CONTENT_TYPE_HTML);
        return;
    }

    // issues.html
    if (req.path == std::string("/") + HtmlGenerator::ISSUES_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "Issues", _database.Issues),
                   CONTENT_TYPE_HTML);
        return;
    }

    // item.html
    if (req.path == std::string("/") + HtmlGenerator::ITEM_HTML)
    {
        const std::string idStr = GetParam(req.params, "id", HtmlGenerator::ITEM_HTML);
        if (idStr.empty())
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                        HtmlGenerator::ITEM_HTML, "id");
            res.set_redirect(HtmlGenerator::INDEX_HTML);
            return;
        }
        const int id = std::stoi(idStr);
        SetContent(req, res, HtmlGenerator::GetItemPage(_database, id), CONTENT_TYPE_HTML);
        return;
    }

    // edit.html
    if (req.path == std::string("/") + HtmlGenerator::EDIT_HTML)
    {
        const std::string filename = GetParam(req.params, "file", HtmlGenerator::EDIT_HTML);
        if (filename.empty())
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                        HtmlGenerator::EDIT_HTML, "file");
            res.set_redirect(HtmlGenerator::INDEX_HTML);
            return;
        }
        SetContent(req, res, HtmlGenerator::GetEditPage(_database, filename), CONTENT_TYPE_HTML);
        return;
    }

    // items.html
    if (req.path == std::string("/") + HtmlGenerator::ITEMS_HTML)
    {
        const std::string yearStr = GetParam(req.params, "year", HtmlGenerator::ITEMS_HTML);
        if (yearStr.empty())
        {
            res.status = 404;
            _errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                        HtmlGenerator::ITEM_HTML, "year");
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
        for (auto& item : _database.Data)
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
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, name, data), CONTENT_TYPE_HTML);
        return;
    }

    // default
    res.status = 404;
}

HttpServer::HttpServer(const fs::path& inputDirectory, const fs::path& ruleSetFile, const fs::path& configFile,
                       const std::string& editor, const std::string& explorer)
    : _server{std::make_unique<httplib::Server>()}
    , _inputDirectory{inputDirectory}
    , _ruleSetFile{ruleSetFile}
    , _configFile{configFile}
    , _editor{editor}
    , _explorer{explorer}
{
    if (!_server->is_valid())
    {
        throw InternalException(__FILE__, __LINE__, "Could not initialize HttpServer");
    }

    // Get root
    _server->Get("/", [](const httplib::Request& /*req*/, httplib::Response& res) {
        res.set_redirect((std::string("/") + HtmlGenerator::INDEX_HTML).c_str());
    });

    // Get Html page
    _server->Get("/.*\\.html", [&](const httplib::Request& req, httplib::Response& res) {
        try
        {
            HandleHtmlRequest(req, res);
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not generate html page {}", GetUrl(req));
        }
    });

    // Get CSS stylesheet
    _server->Get("/(.*\\.css)", [&](const httplib::Request& req, httplib::Response& res) {
        try
        {
            if (!TrySetContentFromCache(req, res))
            {
                fs::path path = fs::current_path() / ".." / "html" / std::string(req.matches[1]);
                std::string css = Utils::ReadFileContent(path);

                SetContentAndSetCache(req, res, css, CONTENT_TYPE_CSS);
            }
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not get stylesheet {}", GetUrl(req));
        }
    });
    
    // Get JS javascript file
    _server->Get("/(.*\\.js)", [&](const httplib::Request& req, httplib::Response& res) {
        try
        {
            if (!TrySetContentFromCache(req, res))
            {
                fs::path path = fs::current_path() / ".." / "html" / std::string(req.matches[1]);
                std::string js = Utils::ReadFileContent(path);

                SetContentAndSetCache(req, res, js, CONTENT_TYPE_JS);
            }
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not get stylesheet {}", GetUrl(req));
        }
    });

    // Get Images
    _server->Get("/(.*\\.png)", [&](const httplib::Request& req, httplib::Response& res) {
        try
        {
            if (!TrySetContentFromCache(req, res))
            {
                std::string name = req.matches[1];
                SetContentAndSetCache(req, res, GetImageContent(name), CONTENT_TYPE_PNG);
            }
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not get image {}", GetUrl(req));
        }
    });
    _server->Get("/(.*\\.ico)", [&](const httplib::Request& req, httplib::Response& res) {
        try
        {
            if (!TrySetContentFromCache(req, res))
            {
                std::string name = req.matches[1];
                SetContentAndSetCache(req, res, GetImageContent(name), CONTENT_TYPE_ICO);
            }
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not get icon {}", GetUrl(req));
        }
    });

    // Save File
    _server->Post((std::string("/") + HtmlGenerator::SAVE_CMD).c_str(), [&](const httplib::Request& req,
                                                                            httplib::Response& res) {
        try
        {
            Utils::PrintTrace("Received save file request");
            const std::string file = GetParam(req.params, "file", HtmlGenerator::SAVE_CMD);
            if (file.empty())
            {
                res.status = 404;
                std::string errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                                       HtmlGenerator::SAVE_CMD, "file");
                res.set_content(HtmlGenerator::GetErrorPage(res.status, errorMessage), CONTENT_TYPE_HTML);
                return;
            }

            std::string content = req.body.substr(8, std::string::npos);
            Utils::WriteFileContent(file, content);
            res.set_redirect(fmt::format("{}?file={}", HtmlGenerator::EDIT_HTML, file).c_str());
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = "Could not exit";
        }
    });

    // Save Rules
    _server->Get((std::string("/") + HtmlGenerator::SAVE_CMD).c_str(), [&](const httplib::Request& /*unused*/,
                                                                            httplib::Response& res) {
        try
        {
            Utils::PrintTrace("Received save rules request");
            CsvWriter::Write(ruleSetFile, _database.Rules);
            res.set_redirect(_lastUrl.c_str());
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = "Could not exit";
        }
    });

    // Exit
    _server->Get((std::string("/") + HtmlGenerator::EXIT_CMD).c_str(),
                 [&](const httplib::Request& /*req*/, httplib::Response& /*res*/) {
                     try
                     {
                         Utils::PrintTrace("Received exit request. Shutdown application...");
                         _exitCode = 0;
                         _server->stop();
                         if (_loadThread)
                         {
                             _loadThread->join();
                         }
                     }
                     catch (const std::exception& e)
                     {
                         _errorStatus = 500;
                         _errorMessage = e.what();
                     }
                     catch (...)
                     {
                         _errorStatus = 500;
                         _errorMessage = "Could not exit";
                     }
                 });

    // Clear cache, reload
    _server->Get((std::string("/") + HtmlGenerator::RELOAD_CMD).c_str(),
                 [&](const httplib::Request& /*req*/, httplib::Response& /*res*/) {
                     try
                     {
                         Utils::PrintTrace("Received reload request. Restart...");
                         Utils::ResetIdGenerator();
                         _exitCode = 1;
                         _server->stop();
                         if (_loadThread)
                         {
                             _loadThread->join();
                         }
                     }
                     catch (const std::exception& e)
                     {
                         _errorStatus = 500;
                         _errorMessage = e.what();
                     }
                     catch (...)
                     {
                         _errorStatus = 500;
                         _errorMessage = "Could not restart";
                     }
                 });

    // Copy Samples
    _server->Get((std::string("/") + HtmlGenerator::COPY_SAMPLES_CMD).c_str(),
                 [&](const httplib::Request& /*req*/, httplib::Response& res) {
                     try
                     {
                         Utils::PrintTrace("Received copy samples request. Copy and reload...");
                         std::error_code ec;
                         fs::path src = fs::absolute("../test_data/rules_match_test/input/ABC");
                         fs::path dest = _inputDirectory / "ABC";
                         fs::copy(src, dest, fs::copy_options::recursive, ec);
                         if (ec.value() != 0)
                         {
                             res.status = 500;
                             _errorMessage = fmt::format("Could not copy folder '{}' to '{}' ({})", src.string(),
                                                         dest.string(), ec.message());
                             return;
                         }

                         ec.clear();
                         src = fs::absolute("../test_data/rules_match_test/rules_match_test.csv");
                         dest = _ruleSetFile;
                         fs::copy_file(src, dest, fs::copy_options::overwrite_existing, ec);
                         if (ec.value() != 0)
                         {
                             res.status = 500;
                             _errorMessage = fmt::format("Could not copy file '{}' to '{}' ({})", src.string(),
                                                         dest.string(), ec.message());
                             return;
                         }

                         res.set_redirect(HtmlGenerator::RELOAD_CMD);
                     }
                     catch (const std::exception& e)
                     {
                         _errorStatus = 500;
                         _errorMessage = e.what();
                     }
                     catch (...)
                     {
                         _errorStatus = 500;
                         _errorMessage = "Could not copy samples";
                     }
                 });

    // open folder
    _server->Get((std::string("/") + HtmlGenerator::OPEN_CMD).c_str(), [&](const httplib::Request& req,
                                                                           httplib::Response& res) {
        try
        {
            Utils::PrintTrace("Received open folder request. Open folder...");
            const std::string folder = GetParam(req.params, "folder", HtmlGenerator::OPEN_CMD);
            if (folder.empty())
            {
                res.status = 404;
                std::string errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                                       HtmlGenerator::OPEN_CMD, "folder");

                Utils::PrintInfo(fmt::format("Last request: {}", GetUrl(req)));
                res.set_content(HtmlGenerator::GetErrorPage(res.status, errorMessage), CONTENT_TYPE_HTML);
                return;
            }
            Utils::RunSync(_explorer, {fs::absolute(folder).string()});
            res.set_redirect(_lastUrl.c_str());
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not open folder {}", GetUrl(req));
        }
    });

    // new rule
    _server->Get((std::string("/") + HtmlGenerator::NEW_CMD).c_str(), [&](const httplib::Request& req,
                                                                             httplib::Response& res) {
        try
        {
            Utils::PrintTrace("Received add rule request...");
            const std::string idStr = GetParam(req.params, "id", HtmlGenerator::NEW_CMD);
            if (idStr.empty())
            {
                res.status = 404;
                std::string errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                                       HtmlGenerator::NEW_CMD, "id");

                Utils::PrintInfo(fmt::format("Last request: {}", GetUrl(req)));
                res.set_content(HtmlGenerator::GetErrorPage(res.status, errorMessage), CONTENT_TYPE_HTML);
                return;
            }

            int id = -1;
            try
            {
                id = std::stoi(idStr);
            }
            catch (const std::exception& e)
            {
                throw InternalException(__FILE__, __LINE__,
                                        fmt::format("Could not convert '{}' to 'int'. ({})", idStr, e.what()));
            }
            int nextId = _database.NewRule(id);
            std::string url = fmt::format("{}?id={}", HtmlGenerator::ITEM_HTML, nextId);
            CsvWriter::Write(ruleSetFile, _database.Rules);
            res.set_redirect(url.c_str());
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not delete rule {}", GetUrl(req));
        }
    });


    // delete rule
    _server->Get((std::string("/") + HtmlGenerator::DELETE_CMD).c_str(), [&](const httplib::Request& req,
                                                                             httplib::Response& res) {
        try
        {
            Utils::PrintTrace("Received delete rule request...");
            const std::string idStr = GetParam(req.params, "id", HtmlGenerator::DELETE_CMD);
            if (idStr.empty())
            {
                res.status = 404;
                std::string errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                                       HtmlGenerator::DELETE_CMD, "id");

                Utils::PrintInfo(fmt::format("Last request: {}", GetUrl(req)));
                res.set_content(HtmlGenerator::GetErrorPage(res.status, errorMessage), CONTENT_TYPE_HTML);
                return;
            }

            int id = -1;
            try
            {
                id = std::stoi(idStr);
            }
            catch (const std::exception& e)
            {
                throw InternalException(__FILE__, __LINE__,
                                        fmt::format("Could not convert '{}' to 'int'. ({})", idStr, e.what()));
            }
            int nextId = _database.DeleteRule(id);
            std::string url;
            if (nextId >= 0)
            {
                url = fmt::format("{}?id={}", HtmlGenerator::ITEM_HTML, nextId);
            }
            else
            {
                url = HtmlGenerator::INDEX_HTML;
            }
            CsvWriter::Write(ruleSetFile, _database.Rules);
            res.set_redirect(url.c_str());
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not delete rule {}", GetUrl(req));
        }
    });

    // input folder
    _server->Get((std::string("/") + HtmlGenerator::INPUT_CMD).c_str(),
                 [&](const httplib::Request& /*req*/, httplib::Response& res) {
                     try
                     {
                         Utils::PrintTrace("Received open input folder request. Open input folder...");
                         Utils::RunSync(_explorer, {fs::absolute(_inputDirectory).string()});
                         res.set_redirect(_lastUrl.c_str());
                     }
                     catch (const std::exception& e)
                     {
                         _errorStatus = 500;
                         _errorMessage = e.what();
                     }
                     catch (...)
                     {
                         _errorStatus = 500;
                         _errorMessage = "Could not open input folder";
                     }
                 });

    // Set Error Handler
    _server->set_error_handler([&](const httplib::Request& req, httplib::Response& res) {
        Utils::PrintInfo(fmt::format("Last request: {}", GetUrl(req)));
        std::string errorMessage
            = _errorMessage.empty() ? httplib::detail::status_message(res.status) : _errorMessage;
        res.set_content(HtmlGenerator::GetErrorPage(res.status, errorMessage), CONTENT_TYPE_HTML);
    });

    // Set Logger
    _server->set_logger([](const httplib::Request& req, const httplib::Response& res) { PrintRequest(req, res); });

    if (_errorMessage.empty())
    {
        // Start LoadThread
        Load();
    }
}

HttpServer::~HttpServer()
{
    _server->stop();
    _server.reset();
}

void HttpServer::Load()
{
    if (_loadThread)
    {
        _loadThread->join();
    }
    _loadThread = std::make_unique<std::thread>([&] {
        try
        {
            std::unique_lock<std::timed_mutex> lock(_databaseMutex, std::chrono::seconds(2));
            if (!lock.owns_lock())
            {
                throw InternalException(__FILE__, __LINE__, "TIMEOUT: Could not aquire database lock.");
            }
            _database.Load(_inputDirectory, _ruleSetFile);
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = "Could not (re-)load csv data.";
        }
    });
}

int HttpServer::Run()
{
    _server->listen("localhost", 80);
    return _exitCode;
}

} // namespace hokee