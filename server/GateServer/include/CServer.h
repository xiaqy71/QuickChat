#ifndef CSERVER_H
#define CSERVER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;           // from <boost/beast.hpp>
namespace http  = beast::http;            // from <boost/beast/http.hpp>
namespace net   = boost::asio;            // from <boost/asio.hpp>
using tcp       = boost::asio::ip::tcp;   // from <boost/asio/ip/tcp.hpp>

class CServer : public std::enable_shared_from_this<CServer>
{
public:
    CServer(boost::asio::io_context& ioc, unsigned short& port);
    void Start();

private:
    tcp::acceptor                acceptor_;
    net::io_context&             ioc_;
    boost::asio::ip::tcp::socket socket_;
};


#endif