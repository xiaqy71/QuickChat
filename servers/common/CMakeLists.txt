cmake_minimum_required(VERSION 3.16)

file(GLOB COMMON_SRCS "src/*.cpp")
add_library(common STATIC ${COMMON_SRCS})


target_include_directories(common PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)
target_link_libraries(common PUBLIC Boost::filesystem)