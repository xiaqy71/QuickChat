#include "StatusServiceImpl.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <climits>
#include "ConfigMgr.h"
#include "const.h"

std::string generate_unique_string() {
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  std::string unique_string = boost::uuids::to_string(uuid);
  return unique_string;
}

StatusServiceImpl::StatusServiceImpl() : server_index_(0) {
  auto& cfg = ConfigMgr::Inst();
  ChatServer server;
  server.port = cfg["ChatServer1"]["Port"];
  server.host = cfg["ChatServer2"]["Host"];
  servers_.emplace_back(server);

  server.port = cfg["ChatServer2"]["Port"];
  server.host = cfg["ChatServer2"]["Host"];
  servers_.emplace_back(server);
}

Status StatusServiceImpl::GetChatServer(ServerContext* context,
                                        const GetChatServerReq* request,
                                        GetChatServerRsp* reply) {
  std::string prefix("QuickChat Status server has received: ");
  server_index_ = (server_index_++) % (servers_.size());
  auto& server = servers_[server_index_];
  reply->set_host(server.host);
  reply->set_port(server.port);
  reply->set_error(ErrorCodes::Success);
  reply->set_token(generate_unique_string());
  return Status::OK;
}
