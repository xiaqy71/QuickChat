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
find_package(hiredis REQUIRED)

# 查找 Boost
find_package(Boost REQUIRED COMPONENTS filesystem)  # 指定需要的组件
if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION} at ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found. Please install Boost.")
endif()

# 检查 clang-format
find_program(CLANG_FORMAT_EXECUTABLE clang-format)
if(NOT CLANG_FORMAT_EXECUTABLE)
    message(WARNING "clang-format not found. Code formatting will be skipped.")
endif()

# python
set(VENV_PATH "${CMAKE_SOURCE_DIR}/servers/VerifyServer/.venv")
set(PYTHON_EXECUTABLE "${VENV_PATH}/bin/python3")
if(NOT EXISTS ${PYTHON_EXECUTABLE})
    message(FATAL_ERROR "Python exectuable not found in virtual enviroment: ${PYTHON_EXECTUABLE}")
endif()
message(STATUS "Using Python: ${PYTHON_EXECUTABLE}")

# Proto 文件路径
get_filename_component(msg_proto "${CMAKE_SOURCE_DIR}/protos/message.proto" ABSOLUTE)
get_filename_component(msg_proto_path "${msg_proto}" PATH)

# 生成 C++ 的 Protobuf 和 gRPC 文件
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

#生成 Python 的 Protobuf 和 gRPC 文件
set(python_proto_out "${CMAKE_SOURCE_DIR}/servers/VerifyServer/generated")
file(MAKE_DIRECTORY ${python_proto_out})
set(python_msg_pb2 "${python_proto_out}/message_pb2.py")
set(python_msg_pb2_grpc "${python_proto_out}/message_pb2_grpc.py")

add_custom_command(
    OUTPUT ${python_msg_pb2} ${python_msg_pb2_grpc}
    COMMAND ${PYTHON_EXECUTABLE} -m grpc_tools.protoc
            -I${msg_proto_path}
            --python_out=${python_proto_out}
            --grpc_python_out=${python_proto_out}
            ${msg_proto}
    DEPENDS ${msg_proto}
    COMMENT "Generating Python Protobuf and gRPC files from ${msg_proto}"
)

# 复制 common.ini 到 VerifyServer
set(COMMON_INI_SRC "${CMAKE_SOURCE_DIR}/config/common.ini")
set(COMMON_INI_DEST "${CMAKE_SOURCE_DIR}/servers/VerifyServer/common.ini")
if(EXISTS ${COMMON_INI_SRC})
    add_custom_command(
        OUTPUT ${COMMON_INI_DEST}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${COMMON_INI_SRC}" "${COMMON_INI_DEST}"
        DEPENDS ${COMMON_INI_SRC}
        COMMENT "Copying common.ini to VerifyServer"
    )
    add_custom_target(copy_common_ini ALL
        DEPENDS ${COMMON_INI_DEST}
    )
else()
    message(WARNING "common.ini not found at ${COMMON_INI_SRC}")
endif()

# 共享的 msg_grpc_proto 库
add_library(msg_grpc_proto ${msg_proto_srcs} ${msg_grpc_srcs})
target_include_directories(msg_grpc_proto PUBLIC "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>")
target_link_libraries(msg_grpc_proto PUBLIC
    gRPC::grpc++_reflection
    gRPC::grpc++
    protobuf::libprotobuf
)

# 添加自定义目标确保 Python 文件生成
add_custom_target(generate_python_grpc ALL
    DEPENDS ${python_msg_pb2} ${python_msg_pb2_grpc}
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