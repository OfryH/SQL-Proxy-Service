cmake_minimum_required(VERSION 3.10)
project(sql_proxy)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(sql_tests
    tests/test_analyzer.cpp
    src/analyzer/SqlAnalyzer.cpp
)