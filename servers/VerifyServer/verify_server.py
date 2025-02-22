import asyncio
import logging
import grpc
import redis.asyncio as redis
import random
import string
from generated.message_pb2 import GetVerifyRsp, GetVerifyReq
from generated.message_pb2_grpc import VerifyServiceServicer, add_VerifyServiceServicer_to_server
import configparser

# 异步 Redis 客户端
redis_cli = None
_cleanup_coroutines = []

class VerifyService(VerifyServiceServicer):
    async def GetVerifyCode(self, request: GetVerifyReq, context: grpc.aio.ServicerContext)->GetVerifyRsp:
        email = request.email
        logging.info(f"Received request for email: {email}")
        code = "".join(random.choices(string.digits, k=6))
        try:
            await redis_cli.setex(f"verify:{email}", 300, code)
            print(f"Stored code {code} for {email} in Redis")
            return GetVerifyRsp(error=0, email=request.email, code=code)
        except Exception as e:
            print(f"Redis error: {e}")
            context.set_code(grpc.StatusCode.INTERNAL)
            context.set_details("Failed to store verification code")
            return GetVerifyRsp(error=0, email=request.email, code="")
            
    
async def serve() -> None:
    global redis_cli
    
    config = configparser.ConfigParser()
    config.read('common.ini')
    port = config['VerifyServer']['Port']
    redis_host = config.get("Redis", "Host", fallback="localhost")
    redis_port = config.get("Redis", "Port", fallback=6379)
    redis_cli = redis.Redis(host=redis_host, port=redis_port, decode_responses=True)
    try:
        await redis_cli.ping()
        logging.info("Redis connected successfully.")
    except Exception as e:
        logging.error(f"Failed to connect to Redis: {e}")
        return
    
    server = grpc.aio.server()
    add_VerifyServiceServicer_to_server(VerifyService(), server)
    server.add_insecure_port(f'[::]:{port}')
    logging.info(f"VerifyServer started on port {port}...")
    
    await server.start()
    async def server_graceful_shutdown():
        logging.info("Starting graceful shutdown...")
        await server.stop(5)
        await redis_cli.aclose()
    
    _cleanup_coroutines.append(server_graceful_shutdown())
    await server.wait_for_termination()
    
if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    loop = asyncio.get_event_loop()
    try:
        loop.run_until_complete(serve())
    except KeyboardInterrupt:
        logging.info("Interrupted by user, initiating cleanup...")
    finally:
        loop.run_until_complete(asyncio.gather(*_cleanup_coroutines))
        loop.close()
    