cmake_minimum_required(VERSION 3.10)
project(GateServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找工具和依赖
find_program(Protobuf_PROTOC_EXECUTABLE protoc REQUIRED)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads REQUIRED)

# Proto 文件路径
get_filename_component(msg_proto "../../protos/message.proto" ABSOLUTE)
get_filename_component(msg_proto_path "${msg_proto}" PATH)

# 生成的源文件和头文件
set(msg_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/message.pb.cc")
set(msg_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/message.pb.h")
set(msg_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/message.grpc.pb.cc")
set(msg_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/message.grpc.pb.h")

# 添加生成 Protobuf 和 gRPC 文件的自定义命令
add_custom_command(
    OUTPUT ${msg_proto_srcs} ${msg_proto_hdrs} ${msg_grpc_srcs} ${msg_grpc_hdrs}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
         --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
         -I "${msg_proto_path}"
         --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_EXECUTABLE}"
         "${msg_proto}"
    DEPENDS ${msg_proto}
    COMMENT "Generating Protobuf and gRPC files from ${msg_proto}"
)

# msg_grpc_proto 库
add_library(msg_grpc_proto ${msg_proto_srcs} ${msg_grpc_srcs})
target_include_directories(msg_grpc_proto PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")
target_link_libraries(msg_grpc_proto PUBLIC
    gRPC::grpc++_reflection
    gRPC::grpc++
    protobuf::libprotobuf
)

# GateServer 可执行文件
file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "src/*.cpp")
add_executable(GateServer main.cpp ${SRCS})
target_include_directories(GateServer PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)
target_link_libraries(GateServer PRIVATE
    jsoncpp
    Threads::Threads
    msg_grpc_proto
)