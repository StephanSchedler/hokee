#include "HttpServer.h"
#include "Filesystem.h"
#include "InternalException.h"
#include "Settings.h"
#include "Utils.h"
#include "csv/CsvWriter.h"

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
        _errorStatus = 200;
        return;
    }

    // backup.html
    if (req.path == std::string("/") + HtmlGenerator::BACKUP_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetBackupPage(_database, _ruleSetFile), CONTENT_TYPE_HTML);
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
        if (save)
        {
            config.Save(fs::absolute(_configFile));
            res.set_redirect((std::string(HtmlGenerator::SETTINGS_HTML) + "?saved").c_str());
        }
        else
        {
            SetContent(req, res,
                       HtmlGenerator::GetSettingsPage(_database, fs::absolute(_configFile),
                                                      req.params.find("saved") != req.params.end()),
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
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "All items", _database.Data, 0),
                   CONTENT_TYPE_HTML);
        return;
    }

    // assigned.html
    if (req.path == std::string("/") + HtmlGenerator::ASSIGNED_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "Assigned items", _database.Assigned, 0),
                   CONTENT_TYPE_HTML);
        return;
    }

    // unassigned.html
    if (req.path == std::string("/") + HtmlGenerator::UNASSIGNED_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "Unassigned items", _database.Unassigned, 0),
                   CONTENT_TYPE_HTML);
        return;
    }

    // rules.html
    if (req.path == std::string("/") + HtmlGenerator::RULES_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "Rules", _database.Rules, 0),
                   CONTENT_TYPE_HTML);
        return;
    }

    // issues.html
    if (req.path == std::string("/") + HtmlGenerator::ISSUES_HTML)
    {
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, "Issues", _database.Issues, 0),
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
        int flag = 0;
        if (req.params.find("saved") != req.params.end())
        {
            flag = 1;
        }
        else if (req.params.find("failed") != req.params.end())
        {
            flag = -1;
        }

        SetContent(req, res, HtmlGenerator::GetItemPage(_database, id, flag), CONTENT_TYPE_HTML);
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
        SetContent(req, res,
                   HtmlGenerator::GetEditPage(_database, filename, req.params.find("saved") != req.params.end()),
                   CONTENT_TYPE_HTML);
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
            res.set_redirect(HtmlGenerator::INDEX_HTML);
            return;
        }
        const int year = std::stoi(yearStr);
        std::string monthStr = GetParam(req.params, "month", HtmlGenerator::ITEMS_HTML);
        if (monthStr.empty())
        {
            monthStr = "0";
        }
        const int month = std::stoi(monthStr);
        std::string filterStr = GetParam(req.params, "filter", HtmlGenerator::ITEMS_HTML);
        if (filterStr.empty())
        {
            filterStr = "0";
        }
        const int filter = std::stoi(filterStr);
        const std::string cat = GetParam(req.params, "category", HtmlGenerator::ITEMS_HTML);

        CsvTable data{};
        for (auto& item : _database.Data)
        {
            if (year == item->Date.GetYear() && (month == 0 || month == item->Date.GetMonth())
                && (cat == "" || cat == item->Category))
            {
                if (filter == 0 || (filter < 0 && item->Value.ToDouble() < 0)
                    || (filter > 0 && item->Value.ToDouble() >= 0))
                {
                    data.push_back(item);
                }
            }
        }

        std::string name = fmt::format("{}-{}: {}", month, year, cat);
        if (month == 0)
        {
            name = fmt::format("{}: {}", year, cat);
        }
        SetContent(req, res, HtmlGenerator::GetTablePage(_database, name, data, filter), CONTENT_TYPE_HTML);
        return;
    }

    // default
    res.status = 404;
}

HttpServer::HttpServer(const fs::path& inputDirectory, const fs::path& ruleSetFile, const fs::path& configFile,
                       const std::string& explorer)
    : _server{std::make_unique<httplib::Server>()}
    , _inputDirectory{inputDirectory}
    , _ruleSetFile{ruleSetFile}
    , _configFile{configFile}
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
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not generate html page {}", GetUrl(req));
            res.set_redirect(HtmlGenerator::INDEX_HTML);
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
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not get stylesheet {}", GetUrl(req));
            res.set_redirect(HtmlGenerator::INDEX_HTML);
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
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not get js {}", GetUrl(req));
            res.set_redirect(HtmlGenerator::INDEX_HTML);
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
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not get image {}", GetUrl(req));
            res.set_redirect(HtmlGenerator::INDEX_HTML);
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
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not get icon {}", GetUrl(req));
            res.set_redirect(HtmlGenerator::INDEX_HTML);
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
            res.set_redirect(fmt::format("{}?file={}&saved", HtmlGenerator::EDIT_HTML, file).c_str());
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = "Could not save file";
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
    });

    // Save Rules
    _server->Get((std::string("/") + HtmlGenerator::SAVE_RULE_CMD).c_str(), [&](const httplib::Request& req,
                                                                                httplib::Response& res) {
        try
        {
            std::string id = GetParam(req.params, "id", HtmlGenerator::SAVE_RULE_CMD);
            if (id.empty())
            {
                throw InternalException(__FILE__, __LINE__, "Could not get request parameter 'id'.");
            }
            std::string dateFormat = GetParam(req.params, "format", HtmlGenerator::SAVE_RULE_CMD);
            if (dateFormat.empty())
            {
                throw InternalException(__FILE__, __LINE__, "Could not get request parameter 'format'.");
            }
            std::shared_ptr<CsvItem> rule = nullptr;
            for (auto& r : _database.Rules)
            {
                if (fmt::format("{}", r->Id) == id)
                {
                    rule = r;
                }
            }
            if (rule == nullptr)
            {
                throw InternalException(__FILE__, __LINE__, fmt::format("Could not find rule with id={}.", id));
            }

            auto value = GetParam(req.params, "Category", HtmlGenerator::SAVE_RULE_CMD);
            rule->Category = value;
            value = GetParam(req.params, "Account", HtmlGenerator::SAVE_RULE_CMD);
            rule->Account = value;
            bool success = true;
            value = GetParam(req.params, "Date", HtmlGenerator::SAVE_RULE_CMD);
            auto ruleBackup = rule->Date;
            try
            {
                rule->Date = CsvDate(dateFormat, value);
            }
            catch (std::runtime_error&)
            {
                success = false;
                rule->Date = ruleBackup;
            }
            value = GetParam(req.params, "Description", HtmlGenerator::SAVE_RULE_CMD);
            rule->Description = value;
            value = GetParam(req.params, "PayerPayee", HtmlGenerator::SAVE_RULE_CMD);
            rule->PayerPayee = value;
            value = GetParam(req.params, "Type", HtmlGenerator::SAVE_RULE_CMD);
            rule->Type = value;
            value = GetParam(req.params, "Value", HtmlGenerator::SAVE_RULE_CMD);
            auto valueBackup = rule->Value;
            try
            {
                rule->Value = CsvValue(value, "???", -1, false);
            }
            catch (InternalException&)
            {
                success = false;
                rule->Value = valueBackup;
            }

            _database.MatchRules();
            CsvWriter::Write(ruleSetFile, _database.Rules);
            res.set_redirect(
                (fmt::format("{}?id={}&{}", HtmlGenerator::ITEM_HTML, id, success ? "saved" : "failed").c_str()));
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = "Could not save rule";
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
    });

    // Save File
    _server->Get((std::string("/") + HtmlGenerator::SAVE_CMD).c_str(),
                 [&](const httplib::Request& /*unused*/, httplib::Response& res) {
                     try
                     {
                         Utils::PrintTrace("Received save rules request");
                         CsvWriter::Write(ruleSetFile, _database.Rules);
                         res.set_redirect((_lastUrl + "&saved").c_str());
                     }
                     catch (const std::exception& e)
                     {
                         _errorStatus = 500;
                         _errorMessage = e.what();
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
                     }
                     catch (...)
                     {
                         _errorStatus = 500;
                         _errorMessage = "Could not save file";
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
                     }
                 });

    // restore folder
    _server->Get((std::string("/") + HtmlGenerator::RESTORE_CMD).c_str(), [&](const httplib::Request& req,
                                                                              httplib::Response& res) {
        try
        {
            Utils::PrintTrace("Received restore request...");
            const fs::path file = GetParam(req.params, "file", HtmlGenerator::RESTORE_CMD);
            if (file.empty())
            {
                res.status = 404;
                std::string errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                                       HtmlGenerator::RESTORE_CMD, "file");

                Utils::PrintInfo(fmt::format("Last request: {}", GetUrl(req)));
                res.set_content(HtmlGenerator::GetErrorPage(res.status, errorMessage), CONTENT_TYPE_HTML);
                return;
            }

            fs::copy_file(ruleSetFile.parent_path() / file, ruleSetFile, fs::copy_options::overwrite_existing);
            res.set_redirect(HtmlGenerator::RELOAD_CMD);
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not restore folder {}", GetUrl(req));
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
    });

    // Backup Rules
    _server->Get((std::string("/") + HtmlGenerator::BACKUP_CMD).c_str(), [&](const httplib::Request& /*unused*/,
                                                                             httplib::Response& res) {
        try
        {
            Utils::PrintTrace("Received backup rules request");
            fs::path backupPath = fmt::format("{}.{}.backup", ruleSetFile.string(), Utils::GenerateTimestamp());
            CsvWriter::Write(ruleSetFile, _database.Rules);
            fs::copy_file(ruleSetFile, backupPath, fs::copy_options::overwrite_existing);
            res.set_redirect((_lastUrl).c_str());
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = "Could not backup";
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
    });

    // Exit
    _server->Get((std::string("/") + HtmlGenerator::EXIT_CMD).c_str(),
                 [&](const httplib::Request& /*req*/, httplib::Response& res) {
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
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
                     }
                     catch (...)
                     {
                         _errorStatus = 500;
                         _errorMessage = "Could not exit";
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
                     }
                 });

    // Clear cache, reload
    _server->Get((std::string("/") + HtmlGenerator::RELOAD_CMD).c_str(),
                 [&](const httplib::Request& /*req*/, httplib::Response& res) {
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
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
                     }
                     catch (...)
                     {
                         _errorStatus = 500;
                         _errorMessage = "Could not restart";
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
                     }
                 });

    // Copy Samples
    _server->Get((std::string("/") + HtmlGenerator::COPY_SAMPLES_CMD).c_str(),
                 [&](const httplib::Request& /*req*/, httplib::Response& res) {
                     try
                     {
                         Utils::PrintTrace("Received copy samples request. Copy and reload...");
                         std::error_code ec;
                         fs::path src = fs::absolute("../test_data/input1/ABC");
                         fs::path dest = _inputDirectory / "ABC";
                         fs::copy(src, dest, fs::copy_options::recursive, ec);
                         if (ec.value() != 0)
                         {
                             res.status = 500;
                             _errorMessage = fmt::format("Could not copy folder '{}' to '{}' ({})", src.string(),
                                                         dest.string(), ec.message());
                             res.set_redirect(HtmlGenerator::INDEX_HTML);
                             return;
                         }

                         ec.clear();
                         src = fs::absolute("../test_data/rules.csv");
                         dest = _ruleSetFile;
                         fs::copy_file(src, dest, fs::copy_options::overwrite_existing, ec);
                         if (ec.value() != 0)
                         {
                             res.status = 500;
                             _errorMessage = fmt::format("Could not copy file '{}' to '{}' ({})", src.string(),
                                                         dest.string(), ec.message());
                             res.set_redirect(HtmlGenerator::INDEX_HTML);
                             return;
                         }

                         res.set_redirect(HtmlGenerator::RELOAD_CMD);
                     }
                     catch (const std::exception& e)
                     {
                         _errorStatus = 500;
                         _errorMessage = e.what();
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
                     }
                     catch (...)
                     {
                         _errorStatus = 500;
                         _errorMessage = "Could not copy samples";
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
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
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not open folder {}", GetUrl(req));
            res.set_redirect(HtmlGenerator::INDEX_HTML);
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
            Utils::PrintInfo(fmt::format("Create new Rule based on id {}", id));
            int nextId = _database.NewRule(id);
            _database.MatchRules();
            CsvWriter::Write(ruleSetFile, _database.Rules);

            std::string url = fmt::format("{}?id={}&saved", HtmlGenerator::ITEM_HTML, nextId);
            res.set_redirect(url.c_str());
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not delete rule {}", GetUrl(req));
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
    });

    // delete rule
    _server->Get((std::string("/") + HtmlGenerator::DELETE_CMD).c_str(), [&](const httplib::Request& req,
                                                                             httplib::Response& res) {
        try
        {
            Utils::PrintTrace("Received delete rule request...");
            const std::string idStr = GetParam(req.params, "id", HtmlGenerator::DELETE_CMD);
            const std::string file = GetParam(req.params, "file", HtmlGenerator::DELETE_CMD);
            if (!idStr.empty())
            {
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
                _database.MatchRules();
                CsvWriter::Write(ruleSetFile, _database.Rules);
                res.set_redirect(url + "&saved");
            }
            else if (!file.empty())
            {
                fs::remove(ruleSetFile.parent_path() / file);
                res.set_redirect(_lastUrl.c_str());
            }
            else
            {
                res.status = 404;
                std::string errorMessage = fmt::format("'{}' requests must define non-empty parameter '{}'!",
                                                       HtmlGenerator::DELETE_CMD, "id' or 'file");

                Utils::PrintInfo(fmt::format("Last request: {}", GetUrl(req)));
                res.set_content(HtmlGenerator::GetErrorPage(res.status, errorMessage), CONTENT_TYPE_HTML);
                return;
            }
        }
        catch (const std::exception& e)
        {
            _errorStatus = 500;
            _errorMessage = e.what();
            res.set_redirect(HtmlGenerator::INDEX_HTML);
        }
        catch (...)
        {
            _errorStatus = 500;
            _errorMessage = fmt::format("Could not delete rule {}", GetUrl(req));
            res.set_redirect(HtmlGenerator::INDEX_HTML);
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
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
                     }
                     catch (...)
                     {
                         _errorStatus = 500;
                         _errorMessage = "Could not open input folder";
                         res.set_redirect(HtmlGenerator::INDEX_HTML);
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