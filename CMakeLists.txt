cmake_minimum_required(VERSION 3.21)
project(TMsgNow)

set(CMAKE_CXX_STANDARD 20)

# add extra include directories
include_directories(/opt/homebrew/Cellar/boost/1.78.0_1/include)

include_directories(.)

add_executable(TMsgNow
        client.cpp
        server.cpp)