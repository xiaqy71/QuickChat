#include "CServer.h"
#include <boost/asio.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    try {
        unsigned short          port = static_cast<unsigned short>(8080);
        net::io_context         ioc{1};
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) { return; }
            ioc.stop();
        });
        std::make_shared<CServer>(ioc, port)->Start();
        ioc.run();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}