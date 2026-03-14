cmake_minimum_required(VERSION 3.3)

project(webserver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/res_man.cpp
    src/setup.cpp
)

add_executable(webserver ${SOURCES})

target_include_directories(webserver PRIVATE src)
target_compile_options(webserver PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
