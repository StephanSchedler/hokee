#pragma once

#include "html/HtmlGenerator.h"
#include "Settings.h"
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace httplib
{
class Server;
struct Request;
struct Response;
} // namespace httplib

namespace hokee
{
class HttpServer
{
    static constexpr const char* CONTENT_TYPE_HTML = "text/html";
    static constexpr const char* CONTENT_TYPE_CSS = "text/css";
    static constexpr const char* CONTENT_TYPE_JS = "application/javascript";
    static constexpr const char* CONTENT_TYPE_PNG = "image/png";
    static constexpr const char* CONTENT_TYPE_ICO = "image/x-icon";

    std::unique_ptr<httplib::Server> _server;
    CsvDatabase _database{};
    std::unordered_map<std::string, std::pair<std::string, std::string>> _cache{};
    std::mutex _cacheMutex{};
    std::string _lastUrl{"/"};
    fs::path _inputDirectory{};
    fs::path _ruleSetFile{};
    fs::path _configFile{};
    std::string _explorer{};
    std::string _errorMessage{};
    std::atomic<int> _errorStatus{200};
    std::unique_ptr<std::thread> _loadThread{nullptr};
    std::timed_mutex _databaseMutex{};
    int _exitCode{0};
    int _port{0};

    void Load();
    void SetContent(const httplib::Request& req, httplib::Response& res, const std::string& content,
                    const char* content_type);
    bool TrySetContentFromCache(const httplib::Request& req, httplib::Response& res);
    void SetContentAndSetCache(const httplib::Request& req, httplib::Response& res, const std::string& content,
                               const char* content_type);
    void HandleHtmlRequest(const httplib::Request& req, httplib::Response& res);

  public:
    HttpServer() = delete;
    HttpServer(const fs::path& inputDirectory, const fs::path& ruleSetFile, const fs::path& configFile,
               const Settings& settings);
    ~HttpServer();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    HttpServer(HttpServer&&) = delete;
    HttpServer& operator=(HttpServer&&) = delete;

    int Run();
    int GetPort() const;
};

} // namespace hokee