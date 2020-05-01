
add_executable(basic runtime/basic.cpp)

target_link_libraries(basic
    sqlpp_dyn sqlite3
)

add_test(
    NAME check_basic
    COMMAND basic
)
