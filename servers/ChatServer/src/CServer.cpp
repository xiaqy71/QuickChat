#include "CServer.h"
#include <iostream>
#include "AsioIOServicePool.h"
#include "CSession.h"
#include "UserMgr.h"

CServer::CServer(boost::asio::io_context& io_context, short port)
    : io_context_(io_context),
      port_(port),
      acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
  std::cout << "Server start success, listen on port: " << port_ << std::endl;
  StartAccept();
}

CServer::~CServer() {
  std::cout << "Server destruct listen on port: " << port_ << std::endl;
}

void CServer::ClearSession(std::string uuid) {
  if (sessions_.find(uuid) != sessions_.end()) {
    UserMgr::GetInstance()->RmvUserSession(sessions_[uuid]->GetUserId());
  }

  {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(uuid);
  }
}

void CServer::HandleAccept(std::shared_ptr<CSession> new_session,
                           const boost::system::error_code& error) {
  if (!error) {
    new_session->Start();
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(std::make_pair(new_session->GetSessionId(), new_session));
  } else {
    std::cout << "session accept failed, error is" << error.what() << std::endl;
  }
  StartAccept();
}

void CServer::StartAccept() {
  auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
  std::shared_ptr<CSession> new_session =
      std::make_shared<CSession>(io_context, this);
  acceptor_.async_accept(new_session->GetSocket(),
                         std::bind(&CServer::HandleAccept, this, new_session,
                                   std::placeholders::_1));
}
