#ifndef STATUSGRPCCLIENT_H
#define STATUSGRPCCLIENT_H

#include <grpcpp/grpcpp.h>
#include "RPConPool.h"
#include "Singleton.h"
#include "message.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

class StatusGrpcClient : public Singleton<StatusGrpcClient> {
  friend class Singleton<StatusGrpcClient>;

 public:
  ~StatusGrpcClient() = default;
  GetChatServerRsp GetChatServer(int uid);

 private:
  StatusGrpcClient();
  std::unique_ptr<RPConPool<StatusService>> pool_;
};

#endif