cmake_minimum_required(VERSION 3.10)
project(GateServer)

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_CROSSCOMPILING)
find_program(_PROTOBUF_PROTOC protoc)
else()
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

if(CMAKE_CROSSCOMPILING)
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

find_package(jsoncpp REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Proto file
get_filename_component(msg_proto "../../protos/message.proto" ABSOLUTE)
get_filename_component(msg_proto_path "${msg_proto}" PATH)

# Generated sources
set(msg_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/message.pb.cc")
set(msg_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/message.pb.h")
set(msg_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/message.grpc.pb.cc")
set(msg_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/message.grpc.pb.h")
add_custom_command(
    OUTPUT "${msg_proto_srcs}" "${msg_proto_hdrs}" "${msg_grpc_srcs}" "${msg_grpc_hdrs}"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
         --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
         -I "${msg_proto_path}"
         --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
         "${msg_proto}"
    DEPENDS "${msg_proto}"
)

# msg_grpc_proto library
add_library(msg_grpc_proto ${msg_proto_srcs} ${msg_grpc_srcs})
target_include_directories(msg_grpc_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(msg_grpc_proto gRPC::grpc++_reflection gRPC::grpc++ protobuf::libprotobuf)

# GateServer executable
file(GLOB SRCS CONFIGURE_DEPENDS "src/*.cpp")
add_executable(GateServer main.cpp ${SRCS})
target_include_directories(GateServer PRIVATE include ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(GateServer PRIVATE jsoncpp pthread msg_grpc_proto)