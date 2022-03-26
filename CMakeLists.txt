cmake_minimum_required(VERSION 3.21)
project(TPlayNow)

set(CMAKE_CXX_STANDARD 20)

# add extra include directories
include_directories(/opt/homebrew/Cellar/boost/1.78.0_1/include)

add_executable(TPlayNow.app main.cpp)

