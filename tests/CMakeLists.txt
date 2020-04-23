cmake_minimum_required(VERSION 3.10.2)

project(Tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")

include_directories(
    ${CMAKE_SOURCE_DIR}/source
)

set(LAUNCHER_SRC
    main.cpp
)

add_executable(launcher ${LAUNCHER_SRC})

target_link_libraries(launcher
    sqlpp_dyn sqlite3
)

add_test(
    NAME Simple
    COMMAND launcher
)
