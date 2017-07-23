cmake_minimum_required(VERSION 3.0.0)

project(Tests)

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runTests "main.cpp")
target_link_libraries(runTests gtest gtest_main pthread)
add_test(runTests runUnitTests )
