#include "VerifyGrpcClient.h"
#include "ConfigMgr.h"

GetVerifyRsp VerifyGrpcClient::GetVerifyCode(const std::string& email) {
  ClientContext context;
  GetVerifyRsp reply;
  GetVerifyReq request;
  request.set_email(email);
  auto stub = pool_->getConnection();
  Status status = stub->GetVerifyCode(&context, request, &reply);

  if (status.ok()) {
    pool_->returnConnection(std::move(stub));
    return reply;
  } else {
    pool_->returnConnection(std::move(stub));
    reply.set_error(ErrorCodes::RPCFailed);
    return reply;
  }
}

VerifyGrpcClient::VerifyGrpcClient() {
  auto& gCfgMgr = ConfigMgr::Inst();
  const std::string& host = gCfgMgr["VerifyServer"]["Host"];
  const std::string& port = gCfgMgr["VerifyServer"]["Port"];
  pool_.reset(new RPConPool<VerifyService>(5, host, port));
}
