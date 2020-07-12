#pragma once

#include "HtmlGenerator.h"
#include <memory>

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
    static CsvDatabase* _pDatabase;
    std::unordered_map<std::string, std::string> _cache{};
    
    static void HandleHtmlRequest(const httplib::Request& req, httplib::Response& res);

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