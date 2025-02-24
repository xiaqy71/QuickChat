#include "CSession.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include "CServer.h"
#include "LogicSystem.h"
#include "MsgNode.h"

CSession::CSession(boost::asio::io_context& io_context, CServer* server)
    : socket_(io_context),
      server_(server),
      b_close_(false),
      b_head_parse_(false),
      user_uid_(0) {
  boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
  session_id_ = boost::uuids::to_string(a_uuid);
  recv_head_node_ = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession() {
  std::cout << "CSession destruct" << std::endl;
}

tcp::socket& CSession::GetSocket() {
  return socket_;
}

std::string& CSession::GetSessionId() {
  return session_id_;
}

void CSession::SetUserId(int uid) {
  user_uid_ = uid;
}

int CSession::GetUserId() {
  return user_uid_;
}

void CSession::Start() {
  AsyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::Send(char* msg, short max_length, short msgid) {
  std::lock_guard<std::mutex> lock(send_lock_);
  int send_que_size = send_que_.size();
  if (send_que_size > MAX_SENDQUE) {
    std::cout << "session: " << session_id_ << " send que fulled, size is"
              << MAX_SENDQUE << std::endl;
    return;
  }
  send_que_.emplace(std::make_shared<SendNode>(msg, max_length, msgid));
  if (send_que_size > 0) {
    return;
  }
  auto& msgnode = send_que_.front();
  boost::asio::async_write(
      socket_, boost::asio::buffer(msgnode->data_, msgnode->total_len_),
      std::bind(&CSession::HandleWrite, this, std::placeholders::_1,
                SharedSelf()));
}

void CSession::Send(std::string msg, short msgid) {
  std::lock_guard<std::mutex> lock(send_lock_);
  int send_que_size = send_que_.size();
  if (send_que_size > MAX_SENDQUE) {
    std::cout << "session: " << session_id_ << " send que fulled. size is "
              << MAX_SENDQUE << std::endl;
    return;
  }
  send_que_.emplace(
      std::make_shared<SendNode>(msg.c_str(), msg.length(), msgid));
  if (send_que_size > 0) {
    return;
  }
  auto& msgnode = send_que_.front();
  boost::asio::async_write(
      socket_, boost::asio::buffer(msgnode->data_, msgnode->total_len_),
      std::bind(&CSession::HandleWrite, this, std::placeholders::_1,
                SharedSelf()));
}

void CSession::Close() {
  socket_.close();
  b_close_ = true;
}

std::shared_ptr<CSession> CSession::SharedSelf() {
  return shared_from_this();
}

void CSession::AsyncReadBody(int total_len) {
  auto self = shared_from_this();
  asyncReadFull(total_len, [self, this, total_len](
                               const boost::system::error_code& ec,
                               std::size_t bytes_transfered) {
    try {
      if (ec) {
        std::cout << "handle read failed, error is " << ec.what() << std::endl;
        Close();
        server_->ClearSession(session_id_);
        return;
      }
      if (bytes_transfered < total_len) {
        std::cout << "read length not match, read [" << bytes_transfered
                  << "], total [" << total_len << "]" << std::endl;
        Close();
        server_->ClearSession(session_id_);
        return;
      }
      memcpy(recv_msg_node_->data_, data_, bytes_transfered);
      recv_msg_node_->cur_len_ += bytes_transfered;
      recv_msg_node_->data_[recv_msg_node_->total_len_] = '\0';
      std::cout << "receive data is " << recv_msg_node_->data_ << std::endl;
      LogicSystem::GetInstance()->PostMsgToQue(
          std::make_shared<LogicNode>(shared_from_this(), recv_msg_node_));
      // 继续监听头部接受事件
      AsyncReadHead(HEAD_TOTAL_LEN);
    } catch (std::exception& e) {
      std::cout << "Exception code is " << e.what() << std::endl;
    }
  });
}

void CSession::AsyncReadHead(int total_len) {
  auto self = shared_from_this();
  asyncReadFull(HEAD_TOTAL_LEN, [self, this](
                                    const boost::system::error_code& ec,
                                    std::size_t bytes_transfered) {
    try {
      if (ec) {
        std::cout << "handle read failed, error is " << ec.what() << std::endl;
        Close();
        server_->ClearSession(session_id_);
        return;
      }
      if (bytes_transfered < HEAD_TOTAL_LEN) {
        std::cout << "read length not match, read [" << bytes_transfered
                  << "], total [" << HEAD_TOTAL_LEN << "]" << std::endl;
        Close();
        server_->ClearSession(session_id_);
        return;
      }
      recv_head_node_->Clear();
      memcpy(recv_head_node_->data_, data_, bytes_transfered);
      // 获取头部MSGID数据
      short msg_id = 0;
      memcpy(&msg_id, recv_head_node_->data_, HEAD_ID_LEN);
      // 网络字节序转化为本地字节序
      msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
      std::cout << "msg_id is" << msg_id << std::endl;
      // id非法
      if (msg_id > MAX_LENGTH) {
        std::cout << "invalid msg_id is" << msg_id << std::endl;
      }
      short msg_len = 0;
      memcpy(&msg_len, recv_head_node_->data_ + HEAD_ID_LEN, HEAD_DATA_LEN);
      // 网络字节序转化为本地字节序
      msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
      std::cout << "msg_len is " << msg_len << std::endl;
      // 长度非法
      if (msg_len > MAX_LENGTH) {
        std::cout << "invalid data length is " << msg_len << std::endl;
        server_->ClearSession(session_id_);
        return;
      }
      recv_msg_node_ = std::make_shared<RecvNode>(msg_len, msg_id);
      AsyncReadBody(msg_len);
    } catch (std::exception& e) {
      std::cout << "Exception code is " << e.what() << std::endl;
    }
  });
}

void CSession::asyncReadFull(
    std::size_t maxLength,
    std::function<void(const boost::system::error_code&, std::size_t)>
        handler) {
  memset(data_, 0, MAX_LENGTH);
  asyncReadLen(0, maxLength, handler);
}

void CSession::asyncReadLen(
    std::size_t read_len, std::size_t total_len,
    std::function<void(const boost::system::error_code&, std::size_t)>
        handler) {
  auto self = shared_from_this();
  socket_.async_read_some(
      boost::asio::buffer(data_ + read_len, total_len - read_len),
      [read_len, total_len, handler, self](const boost::system::error_code& ec,
                                           std::size_t bytesTransfered) {
        if (ec) {
          handler(ec, read_len + bytesTransfered);
          return;
        }
        if (read_len + bytesTransfered >= total_len) {
          handler(ec, read_len + bytesTransfered);
          return;
        }
        self->asyncReadLen(read_len + bytesTransfered, total_len, handler);
      });
}

void CSession::HandleWrite(const boost::system::error_code& error,
                           std::shared_ptr<CSession> shared_self) {
  // 增加异常处理
  try {
    if (!error) {
      std::lock_guard<std::mutex> lock(send_lock_);
      send_que_.pop();
      if (!send_que_.empty()) {
        auto& msgnode = send_que_.front();
        boost::asio::async_write(
            socket_, boost::asio::buffer(msgnode->data_, msgnode->total_len_),
            std::bind(&CSession::HandleWrite, this, std::placeholders::_1,
                      shared_self));
      }
    } else {
      std::cout << "handle write failed, error is " << error.what()
                << std::endl;
      Close();
      server_->ClearSession(session_id_);
    }
  } catch (std::exception& e) {
    std::cerr << "Exception code: " << e.what() << std::endl;
  }
}

LogicNode::LogicNode(std::shared_ptr<CSession> session,
                     std::shared_ptr<RecvNode> recvnode)
    : session_(session), recvnode_(recvnode) {}
