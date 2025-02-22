#include "CServer.h"
#include "HttpConnection.h"
#include <iostream>
CServer::CServer(boost::asio::io_context& ioc, unsigned short& port)
    : ioc_(ioc)
    , acceptor_(ioc, tcp::endpoint(tcp::v4(), port))
    , socket_(ioc)
{}

void CServer::Start()
{
    auto self = shared_from_this();
    acceptor_.async_accept(socket_, [self](beast::error_code ec) {
        try {
            if (ec) {
                self->Start();
                return;
            }
            std::make_shared<HttpConnection>(std::move(self->socket_))->Start();
            self->Start();
        }
        catch (std::exception& e) {
            std::cout << "exception is " << e.what() << std::endl;
            self->Start();
        };
    });
}