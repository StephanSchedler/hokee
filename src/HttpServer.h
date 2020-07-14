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
    std::unique_ptr<httplib::Server> _server = nullptr;
    CsvDatabase* _pDatabase = nullptr;
    std::unordered_map<std::string, std::string> _cache{};
    
    bool SetCacheContent(const httplib::Request& req, httplib::Response& res);
    void SetContent(const httplib::Request& req, httplib::Response& res, const std::string& content, const char* content_type);
    void HandleHtmlRequest(const httplib::Request& req, httplib::Response& res);

  public:
    HttpServer(CsvDatabase* pDatabase);
    ~HttpServer();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    HttpServer(HttpServer&&) = default;
    HttpServer& operator=(HttpServer&&) = default;

    void Run();
};

} // namespace hokee