#include "CServer.h"
#include "AsioIOServicePool.h"
#include "HttpConnection.h"
#include <iostream>
CServer::CServer(boost::asio::io_context& ioc, unsigned short& port)
    : ioc_(ioc)
    , acceptor_(ioc, tcp::endpoint(tcp::v4(), port))
    , socket_(ioc)
{}

void CServer::Start()
{
    auto                            self       = shared_from_this();
    auto&                           io_context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<HttpConnection> new_con    = std::make_shared<HttpConnection>(io_context);
    acceptor_.async_accept(new_con->GetSocket(), [self, new_con](beast::error_code ec) {
        try {
            if (ec) {
                self->Start();
                return;
            }
            new_con->Start();
            self->Start();
        }
        catch (std::exception& e) {
            std::cout << "exception is " << e.what() << std::endl;
            self->Start();
        };
    });
}