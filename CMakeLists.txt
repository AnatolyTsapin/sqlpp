cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")

include_directories(
    ${CMAKE_SOURCE_DIR}
    )

set(SQLPP_SRC
    sqlpp/database.cpp sqlpp/statement.cpp sqlpp/table.cpp sqlpp/column.cpp sqlpp/value.cpp
    sqlpp/result.cpp sqlpp/types.cpp
    )

set(LAUNCHER_SRC
    main.cpp
    )

add_executable(launcher ${LAUNCHER_SRC} ${SQLPP_SRC})

target_link_libraries(launcher
    sqlite3)
    