cmake_minimum_required(VERSION 3.0.0)

project(Cache)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

set(HEADERS Cache.hpp)

add_executable(${PROJECT_NAME} "main.cpp" ${HEADERS})
