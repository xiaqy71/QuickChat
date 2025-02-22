cmake_minimum_required(VERSION 3.10)
project(QuickChat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找全局依赖
find_program(Protobuf_PROTOC_EXECUTABLE protoc REQUIRED)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads REQUIRED)

# 检查 clang-format
find_program(CLANG_FORMAT_EXECUTABLE clang-format)
if(NOT CLANG_FORMAT_EXECUTABLE)
    message(WARNING "clang-format not found. Code formatting will be skipped.")
endif()

# Proto 文件路径
get_filename_component(msg_proto "${CMAKE_SOURCE_DIR}/protos/message.proto" ABSOLUTE)
get_filename_component(msg_proto_path "${msg_proto}" PATH)

# 生成的源文件和头文件
set(msg_proto_srcs "${CMAKE_BINARY_DIR}/message.pb.cc")
set(msg_proto_hdrs "${CMAKE_BINARY_DIR}/message.pb.h")
set(msg_grpc_srcs "${CMAKE_BINARY_DIR}/message.grpc.pb.cc")
set(msg_grpc_hdrs "${CMAKE_BINARY_DIR}/message.grpc.pb.h")

add_custom_command(
    OUTPUT ${msg_proto_srcs} ${msg_proto_hdrs} ${msg_grpc_srcs} ${msg_grpc_hdrs}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out "${CMAKE_BINARY_DIR}"
         --cpp_out "${CMAKE_BINARY_DIR}"
         -I "${msg_proto_path}"
         --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_EXECUTABLE}"
         "${msg_proto}"
    DEPENDS ${msg_proto}
    COMMENT "Generating Protobuf and gRPC files from ${msg_proto}"
)

# 共享的 msg_grpc_proto 库
add_library(msg_grpc_proto ${msg_proto_srcs} ${msg_grpc_srcs})
target_include_directories(msg_grpc_proto PUBLIC "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>")
target_link_libraries(msg_grpc_proto PUBLIC
    gRPC::grpc++_reflection
    gRPC::grpc++
    protobuf::libprotobuf
)

# 格式化代码目标
if(CLANG_FORMAT_EXECUTABLE)
    add_custom_target(format_code
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/format_code.py  # 或 format_code.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source code with clang-format"
    )
    # 使格式化在构建前执行
    add_dependencies(msg_grpc_proto format_code)  # 在生成 proto 文件后格式化
endif()

# 添加子目录
add_subdirectory(servers)