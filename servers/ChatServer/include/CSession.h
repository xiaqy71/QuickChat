#ifndef CSESSION_H
#define CSESSION_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <memory>
#include <queue>
#include "const.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class CServer;
class LogicSystem;
class SendNode;
class MsgNode;
class RecvNode;

class CSession : public std::enable_shared_from_this<CSession> {
 public:
  CSession(boost::asio::io_context& io_context, CServer* server);
  ~CSession();
  tcp::socket& GetSocket();
  std::string& GetSessionId();
  void SetUserId(int uid);
  int GetUserId();
  void Start();
  void Send(char* msg, short max_length, short msgid);
  void Send(std::string msg, short msgid);
  void Close();
  std::shared_ptr<CSession> SharedSelf();
  void AsyncReadBody(int total_len);
  void AsyncReadHead(int total_len);

 private:
  void asyncReadFull(
      std::size_t maxLength,
      std::function<void(const boost::system::error_code&, std::size_t)>
          handler);
  void asyncReadLen(
      std::size_t read_len, std::size_t total_len,
      std::function<void(const boost::system::error_code&, std::size_t)>
          handler);

  void HandleWrite(const boost::system::error_code& error,
                   std::shared_ptr<CSession> shared_self);
  tcp::socket socket_;
  std::string session_id_;
  char data_[MAX_LENGTH];
  CServer* server_;
  bool b_close_;
  std::queue<std::shared_ptr<SendNode>> send_que_;
  std::mutex send_lock_;
  std::shared_ptr<RecvNode> recv_msg_node_;
  bool b_head_parse_;
  std::shared_ptr<MsgNode> recv_head_node_;
  int user_uid_;
};

class LogicNode {
  friend class LogicSystem;

 public:
  LogicNode(std::shared_ptr<CSession>, std::shared_ptr<RecvNode>);

 private:
  std::shared_ptr<CSession> session_;
  std::shared_ptr<RecvNode> recvnode_;
};

#endif