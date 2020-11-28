cmake_minimum_required(VERSION 3.17)
project(dkvs)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/Cellar/thrift/0.13.0/include)

include_directories(/usr/local/include)

include_directories(/usr/local/Cellar/thrift/0.13.0/bin/thrift)

set(CMAKE_CXX_FLAGS "-fsanitize=address -g -Wall -Wextra -pedantic")

add_executable(server gen-cpp/dkvs.cpp gen-cpp/dkvs.h gen-cpp/server.hpp gen-cpp/server.cpp gen-cpp/store_constants.cpp
    gen-cpp/store_constants.h gen-cpp/store_types.cpp gen-cpp/store_types.h)

add_executable(client gen-cpp/dkvs.cpp gen-cpp/dkvs.h gen-cpp/client.cpp gen-cpp/store_constants.cpp
    gen-cpp/store_constants.h gen-cpp/store_types.cpp gen-cpp/store_types.h)