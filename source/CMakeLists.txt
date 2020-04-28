cmake_minimum_required(VERSION 3.16.3)

project(SQLpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQLPP_SRC
    sqlpp/condition.cpp
    sqlpp/database.cpp
    sqlpp/result.cpp
    sqlpp/types.cpp
    sqlpp/stmt/common.cpp
    sqlpp/stmt/create.cpp
    sqlpp/stmt/insert.cpp
    sqlpp/stmt/select.cpp
)

add_library(sqlpp_st ${SQLPP_SRC})
set_target_properties(sqlpp_st PROPERTIES
    OUTPUT_NAME sqlpp
)

add_library(sqlpp_dyn SHARED ${SQLPP_SRC})
set_target_properties(sqlpp_dyn PROPERTIES
    OUTPUT_NAME sqlpp
)

install(TARGETS sqlpp_st sqlpp_dyn
    DESTINATION lib
)

install(FILES sqlpp.h
    DESTINATION include
)
install(DIRECTORY sqlpp
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
)
