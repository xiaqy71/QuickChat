#ifndef VERIFYGRPCCLIENT_H
#define VERIFYGRPCCLIENT_H

#include "RPConPool.h"
#include "Singleton.h"
#include "const.h"
#include "message.grpc.pb.h"
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class VerifyGrpcClient : public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;

public:
    GetVerifyRsp GetVerifyCode(const std::string& email);

private:
    VerifyGrpcClient();
    std::unique_ptr<RPConPool<VerifyService>> pool_;
};


#endif