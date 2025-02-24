#ifndef CSERVER_H
#define CSERVER_H

#include <boost/asio.hpp>
#include <mutex>
#include <unordered_map>
using boost::asio::ip::tcp;

class CSession;

class CServer {
 public:
  CServer(boost::asio::io_context& io_context, short port);
  ~CServer();
  void ClearSession(std::string);

 private:
  void HandleAccept(std::shared_ptr<CSession>,
                    const boost::system::error_code& error);
  void StartAccept();
  boost::asio::io_context& io_context_;
  short port_;
  tcp::acceptor acceptor_;
  std::unordered_map<std::string, std::shared_ptr<CSession>> sessions_;
  std::mutex mutex_;
};

#endif