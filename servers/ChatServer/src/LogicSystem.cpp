#include "LogicSystem.h"
#include "CSession.h"
#include "MsgNode.h"

LogicSystem::LogicSystem() : b_stop_(false) {
  RegisterCallBacks();
  worker_thread_ = std::thread(&LogicSystem::DealMsg, this);
}

LogicSystem::~LogicSystem() {
  b_stop_ = true;
  consume_.notify_one();
  worker_thread_.join();
}

void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg) {
  std::unique_lock<std::mutex> lock(mutex_);
  msg_que_.emplace(msg);
  if (msg_que_.size() == 1) {
    lock.unlock();
    consume_.notify_one();
  }
}

void LogicSystem::DealMsg() {
  for (;;) {
    std::unique_lock<std::mutex> lock(mutex_);
    consume_.wait(lock,
                  [this]() -> bool { return !msg_que_.empty() || b_stop_; });
    if (b_stop_) {
      while (!msg_que_.empty()) {
        auto msg_node = msg_que_.front();
        std::cout << "recv_msg id is " << msg_node->recvnode_->msg_id_
                  << std::endl;
        auto callback_it = fun_callbacks_.find(msg_node->recvnode_->msg_id_);
        if (fun_callbacks_.end() == callback_it) {
          msg_que_.pop();
          continue;
        }
        callback_it->second(msg_node->session_, msg_node->recvnode_->msg_id_,
                            std::string(msg_node->recvnode_->data_,
                                        msg_node->recvnode_->total_len_));
        msg_que_.pop();
      }
      break;
    }

    auto msg_node = msg_que_.front();
    std::cout << "recv_msg id is : " << msg_node->recvnode_->msg_id_
              << std::endl;

    auto callback_it = fun_callbacks_.find(msg_node->recvnode_->msg_id_);
    if (fun_callbacks_.end() == callback_it) {
      msg_que_.pop();
      continue;
    }
    callback_it->second(msg_node->session_, msg_node->recvnode_->msg_id_,
                        std::string(msg_node->recvnode_->data_,
                                    msg_node->recvnode_->total_len_));
    msg_que_.pop();
  }
}

void LogicSystem::RegisterCallBacks() {}
