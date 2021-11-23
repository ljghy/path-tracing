cmake_minimum_required(VERSION 3.2)
project(path-tracing)
set(CMAKE_CXX_FLAGS "-std=c++17 -pthread")
add_compile_options(-Wall)
include_directories(./include)

file(GLOB_RECURSE SRC_LIST "src/*.cpp")
add_executable(path-tracer ${SRC_LIST})
