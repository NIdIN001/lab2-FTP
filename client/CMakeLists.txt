cmake_minimum_required(VERSION 3.17)
project(client)

set(CMAKE_CXX_STANDARD 20)

add_executable(client main.cpp Client.cpp FileManager.cpp)