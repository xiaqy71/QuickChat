# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
"""Client and server classes corresponding to protobuf-defined services."""
import grpc
import warnings

import message_pb2 as message__pb2

GRPC_GENERATED_VERSION = '1.70.0'
GRPC_VERSION = grpc.__version__
_version_not_supported = False

try:
    from grpc._utilities import first_version_is_lower
    _version_not_supported = first_version_is_lower(GRPC_VERSION, GRPC_GENERATED_VERSION)
except ImportError:
    _version_not_supported = True

if _version_not_supported:
    raise RuntimeError(
        f'The grpc package installed is at version {GRPC_VERSION},'
        + f' but the generated code in message_pb2_grpc.py depends on'
        + f' grpcio>={GRPC_GENERATED_VERSION}.'
        + f' Please upgrade your grpc module to grpcio>={GRPC_GENERATED_VERSION}'
        + f' or downgrade your generated code using grpcio-tools<={GRPC_VERSION}.'
    )


class VerifyServiceStub(object):
    """Missing associated documentation comment in .proto file."""

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.GetVerifyCode = channel.unary_unary(
                '/message.VerifyService/GetVerifyCode',
                request_serializer=message__pb2.GetVerifyReq.SerializeToString,
                response_deserializer=message__pb2.GetVerifyRsp.FromString,
                _registered_method=True)


class VerifyServiceServicer(object):
    """Missing associated documentation comment in .proto file."""

    def GetVerifyCode(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_VerifyServiceServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'GetVerifyCode': grpc.unary_unary_rpc_method_handler(
                    servicer.GetVerifyCode,
                    request_deserializer=message__pb2.GetVerifyReq.FromString,
                    response_serializer=message__pb2.GetVerifyRsp.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'message.VerifyService', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))
    server.add_registered_method_handlers('message.VerifyService', rpc_method_handlers)


 # This class is part of an EXPERIMENTAL API.
class VerifyService(object):
    """Missing associated documentation comment in .proto file."""

    @staticmethod
    def GetVerifyCode(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/message.VerifyService/GetVerifyCode',
            message__pb2.GetVerifyReq.SerializeToString,
            message__pb2.GetVerifyRsp.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)


class StatusServiceStub(object):
    """Missing associated documentation comment in .proto file."""

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.GetChatServer = channel.unary_unary(
                '/message.StatusService/GetChatServer',
                request_serializer=message__pb2.GetChatServerReq.SerializeToString,
                response_deserializer=message__pb2.GetChatServerRsp.FromString,
                _registered_method=True)
        self.Login = channel.unary_unary(
                '/message.StatusService/Login',
                request_serializer=message__pb2.LoginReq.SerializeToString,
                response_deserializer=message__pb2.LoginRsp.FromString,
                _registered_method=True)


class StatusServiceServicer(object):
    """Missing associated documentation comment in .proto file."""

    def GetChatServer(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Login(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_StatusServiceServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'GetChatServer': grpc.unary_unary_rpc_method_handler(
                    servicer.GetChatServer,
                    request_deserializer=message__pb2.GetChatServerReq.FromString,
                    response_serializer=message__pb2.GetChatServerRsp.SerializeToString,
            ),
            'Login': grpc.unary_unary_rpc_method_handler(
                    servicer.Login,
                    request_deserializer=message__pb2.LoginReq.FromString,
                    response_serializer=message__pb2.LoginRsp.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'message.StatusService', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))
    server.add_registered_method_handlers('message.StatusService', rpc_method_handlers)


 # This class is part of an EXPERIMENTAL API.
class StatusService(object):
    """Missing associated documentation comment in .proto file."""

    @staticmethod
    def GetChatServer(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/message.StatusService/GetChatServer',
            message__pb2.GetChatServerReq.SerializeToString,
            message__pb2.GetChatServerRsp.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def Login(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/message.StatusService/Login',
            message__pb2.LoginReq.SerializeToString,
            message__pb2.LoginRsp.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)
