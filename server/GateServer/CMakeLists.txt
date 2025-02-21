cmake_minimum_required(VERSION 3.10)

project(GateServer)

set(CMAKE_CXX_STANDARD 14)

find_package(jsoncpp)

include_directories(include)
file(GLOB SRCS "src/*.cpp")

add_executable(GateServer main.cpp ${SRCS} include/Singleton.h)

target_link_libraries(GateServer jsoncpp pthread)

