cmake_minimum_required(VERSION 3.16.3)

project(Tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")

include_directories(
    ${CMAKE_SOURCE_DIR}/source
)

include(compile/compile.cmake)
include(runtime/runtime.cmake)
