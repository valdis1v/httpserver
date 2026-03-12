cmake_minimum_required(VERSION 3.20)

project(web-server)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC_FILES src/*.cpp)

add_executable(webserver ${SRC_FILES})

target_include_directories(webserver PRIVATE src)
