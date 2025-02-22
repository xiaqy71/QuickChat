import grpc
from concurrent import futures
import time
import configparser

from generated.message_pb2 import GetVerifyRsp
from generated.message_pb2_grpc import VerifyServiceServicer, add_VerifyServiceServicer_to_server

class VerifyService(VerifyServiceServicer):
    def GetVerifyCode(self, request, context):
        email = request.email;
        print(f"receive email is {email}")
        code = "123456"
        return GetVerifyRsp(error=0, email=request.email, code=code)
    
def serve():
    config = configparser.ConfigParser()
    config.read('common.ini')
    port = config['VerifyServer']['Port']
    
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    add_VerifyServiceServicer_to_server(VerifyService(), server)
    
    server.add_insecure_port(f'[::]:{port}')
    print(f"VerifyServer started on port {port}...")
    
    server.start()
    try:
        while True:
            time.sleep(86400)
    except KeyboardInterrupt:
        server.stop(0)
        print("VerifyServer stopped.")

if __name__ == "__main__":
    serve()