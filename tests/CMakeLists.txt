cmake_minimum_required(VERSION 3.0.0)

project(Tests)

add_definitions(-DTEST_WITH_BOOST_SHARED_POINTER)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Werror -pedantic")

set(HEADERS ../Cache.hpp)

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ..)

add_executable(runTests "main.cpp" ${HEADERS})
target_link_libraries(runTests gtest gtest_main pthread)
add_test(runTests runUnitTests )
