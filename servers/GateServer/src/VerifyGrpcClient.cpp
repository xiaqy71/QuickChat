#include "VerifyGrpcClient.h"

GetVerifyRsp VerifyGrpcClient::GetVerifyCode(const std::string& email)
{
    ClientContext context;
    GetVerifyRsp  reply;
    GetVerifyReq  request;
    request.set_email(email);
    Status status = stub_->GetVerifyCode(&context, request, &reply);

    if (status.ok()) { return reply; }
    else {
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}

VerifyGrpcClient::VerifyGrpcClient()
{
    std::shared_ptr<Channel> channel =
        grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
    stub_ = VerifyService::NewStub(channel);
}
