#include <grpcpp/grpcpp.h>
#include <boost/asio.hpp>
#include <iostream>
#include "ConfigMgr.h"
#include "StatusServiceImpl.h"

void RunServer() {
  auto& cfg = ConfigMgr::Inst();
  std::string server_address(cfg["StatusServer"]["Host"] + ":" +
                             cfg["StatusServer"]["Port"]);
  StatusServiceImpl service;

  grpc::ServerBuilder builder;
  // 监听端口和添加服务
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  boost::asio::io_context io_context;
  boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);

  signals.async_wait(
      [&server](const boost::system::error_code& error, int signal_number) {
        if (!error) {
          std::cout << "Shutting down server..." << std::endl;
          server->Shutdown();
        }
      });

  std::thread([&io_context]() { io_context.run(); }).detach();

  server->Wait();
  io_context.stop();
}

int main(int argc, char* argv[]) {
  try {
    RunServer();
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}