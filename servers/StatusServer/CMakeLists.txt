# servers/StatusServer/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# StatusServer 可执行文件
file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "src/*.cpp")
add_executable(StatusServer ${SRCS})
target_include_directories(StatusServer PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>"
)
target_link_libraries(StatusServer PRIVATE
    msg_grpc_proto  # 使用顶层定义的共享库
    common          # 共享代码
    jsoncpp
    hiredis
    
)

# 处理 INI 文件
set(CONFIG_FILE "${CMAKE_SOURCE_DIR}/config/common.ini")
if(EXISTS ${CONFIG_FILE})
    configure_file(${CONFIG_FILE} ${CMAKE_CURRENT_BINARY_DIR}/common.ini COPYONLY)
    message(STATUS "Config file ${CONFIG_FILE} copied to ${CMAKE_CURRENT_BINARY_DIR}/common.ini")
else()
    message(WARNING "Config file ${CONFIG_FILE} not found!")
endif()

