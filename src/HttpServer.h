#pragma once

#include "HtmlGenerator.h"
#include <memory>
#include <unordered_map>

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
    CsvDatabase* _pDatabase = nullptr;
    std::unordered_map<std::string, std::pair<std::string, std::string>> _cache{};
    std::string _lastUrl{"/"};
    
    bool SetCacheContent(const httplib::Request& req, httplib::Response& res);
    void SetContent(const httplib::Request& req, httplib::Response& res, const std::string& content, const char* content_type);
    void HandleHtmlRequest(const httplib::Request& req, httplib::Response& res);

  public:
    HttpServer() = delete;
    HttpServer(CsvDatabase* pDatabase);
    ~HttpServer();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    HttpServer(HttpServer&&) = default;
    HttpServer& operator=(HttpServer&&) = default;

    void Run();
};

} // namespace hokee