import grpc
from generated.message_pb2 import GetVerifyReq
from generated.message_pb2_grpc import VerifyServiceStub

def run():
    channel = grpc.insecure_channel('localhost:50051')
    stub = VerifyServiceStub(channel)
    response = stub.GetVerifyCode(GetVerifyReq(email="792155443@qq.com"))
if __name__ == "__main__":
    run()