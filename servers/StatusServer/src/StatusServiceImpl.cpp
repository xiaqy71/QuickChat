#include "StatusServiceImpl.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <climits>
#include <sstream>
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "const.h"

std::string generate_unique_string() {
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  std::string unique_string = boost::uuids::to_string(uuid);
  return unique_string;
}

StatusServiceImpl::StatusServiceImpl() {
  auto& cfg = ConfigMgr::Inst();
  auto server_list = cfg["chatservers"]["Name"];

  std::vector<std::string> words;
  std::stringstream ss(server_list);
  std::string word;

  while (std::getline(ss, word, ',')) {
    words.push_back(word);
  }

  for (auto& word : words) {
    if (cfg[word]["Name"].empty()) {
      continue;
    }

    ChatServer server;
    server.port = cfg[word]["Port"];
    server.host = cfg[word]["Host"];
    server.name = cfg[word]["Name"];
    servers_[server.name] = server;
  }
}

Status StatusServiceImpl::GetChatServer(ServerContext* context,
                                        const GetChatServerReq* request,
                                        GetChatServerRsp* reply) {
  const auto& server = getChatServer();
  reply->set_host(server.host);
  reply->set_port(server.port);
  reply->set_error(ErrorCodes::Success);
  reply->set_token(generate_unique_string());
  insertToken(request->uid(), reply->token());
  return Status::OK;
}

Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request,
                                LoginRsp* reply) {
  auto uid = request->uid();
  auto token = request->token();

  std::string uid_str = std::to_string(uid);
  std::string token_key = USERTOKENPREFIX + uid_str;
  std::string token_value = "";
  bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
  if (success) {
    reply->set_error(ErrorCodes::UidInvalid);
    return Status::OK;
  }

  if (token_value != token) {
    reply->set_error(ErrorCodes::TokenInvalid);
    return Status::OK;
  }

  reply->set_error(ErrorCodes::Success);
  reply->set_uid(uid);
  reply->set_token(token);
  return Status::OK;
}

void StatusServiceImpl::insertToken(int uid, std::string token) {
  std::string uid_str = std::to_string(uid);
  std::string token_key = USERTOKENPREFIX + uid_str;
  RedisMgr::GetInstance()->Set(token_key, token);
}

ChatServer& StatusServiceImpl::getChatServer() {
  std::lock_guard<std::mutex> guard(server_mtx_);
  auto& minServer = servers_.begin()->second;
  auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, minServer.name);
  if (count_str.empty()) {
    minServer.con_count = INT_MAX;
  } else {
    minServer.con_count = std::stoi(count_str);
  }

  for (auto& server : servers_) {
    if (server.second.name == minServer.name) {
      continue;
    }
    auto count_str =
        RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server.second.name);
    if (count_str.empty()) {
      server.second.con_count = INT_MAX;
    } else {
      server.second.con_count = std::stoi(count_str);
    }

    if (server.second.con_count < minServer.con_count) {
      minServer = server.second;
    }
  }

  return minServer;
}

ChatServer::ChatServer() : host(""), port(""), name(""), con_count(0) {}
