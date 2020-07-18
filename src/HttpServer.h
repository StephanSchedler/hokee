#pragma once

#include "HtmlGenerator.h"
#include <memory>
#include <unordered_map>
#include <thread>
#include <mutex>

namespace httplib
{
class Server;
struct Request;
struct Response;
}

namespace hokee
{
class HttpServer
{
    static constexpr const char* CONTENT_TYPE_HTML = "text/html";
    static constexpr const char* CONTENT_TYPE_PNG = "image/png";
    static constexpr const char* CONTENT_TYPE_ICO = "image/x-icon";

    std::unique_ptr<httplib::Server> _server;
    CsvDatabase _database{};
    std::unordered_map<std::string, std::pair<std::string, std::string>> _cache{};
    std::string _lastUrl{"/"};
    fs::path _inputDirectory{};
    fs::path _ruleSetFile{};
    fs::path _configFile{};
    std::string _editor{};
    std::unique_ptr<std::thread>_loadThread{nullptr};
    std::timed_mutex _databaseMutex{};

    void Load();
    bool SetCacheContent(const httplib::Request& req, httplib::Response& res);
    void SetContent(const httplib::Request& req, httplib::Response& res, const std::string& content, const char* content_type);
    void HandleHtmlRequest(const httplib::Request& req, httplib::Response& res);

  public:
    HttpServer() = delete;
    HttpServer(const fs::path& inputDirectory, const fs::path& ruleSetFile,  const fs::path& configFile, const std::string& editor);
    ~HttpServer();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    HttpServer(HttpServer&&) = delete;
    HttpServer& operator=(HttpServer&&) = delete;

    void Run();
};

} // namespace hokee