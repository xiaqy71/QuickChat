#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <unordered_map>

namespace beast = boost::beast;           // from <boost/beast.hpp>
namespace http  = beast::http;            // from <boost/beast/http.hpp>
namespace net   = boost::asio;            // from <boost/asio.hpp>
using tcp       = boost::asio::ip::tcp;   // from <boost/asio/ip/tcp.hpp>

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;

public:
    HttpConnection(tcp::socket socket);
    void Start();

private:
    void CheckDeadline();
    void WriteResponse();
    void HandleReq();
    void PreParseGetParam();

    tcp::socket                        socket_;
    beast::flat_buffer                 buffer_{8192};
    http::request<http::dynamic_body>  request_;
    http::response<http::dynamic_body> response_;
    net::steady_timer                  deadline_;
    std::string get_url_;
    std::unordered_map<std::string, std::string> get_params_;
};


#endif