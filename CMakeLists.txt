cmake_minimum_required(VERSION 3.21)
project(TMsgNow)

set(CMAKE_CXX_STANDARD 14)

# add extra include directories
include_directories(/opt/homebrew/Cellar/boost/1.78.0_1/include)

add_executable(TMsgNow sample_implementing_server/asynchronous_tcp_server.cpp)
