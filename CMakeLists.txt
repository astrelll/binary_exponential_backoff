cmake_minimum_required(VERSION 3.8)
project(binary_exponential_backoff)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(binary_exponential_backoff ${SOURCE_FILES})
