#include "CServer.h"
#include "ConfigMgr.h"
#include <boost/asio.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    try {
        std::string             gate_port_str = ConfigMgr::Inst()["GateServer"]["Port"];
        unsigned short          gate_port     = atoi(gate_port_str.c_str());
        net::io_context         ioc{1};
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) { return; }
            ioc.stop();
        });
        std::make_shared<CServer>(ioc, gate_port)->Start();
        ioc.run();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}