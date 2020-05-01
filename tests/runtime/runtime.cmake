macro(add_run_test TEST_NAME)
    add_executable(${TEST_NAME} runtime/${TEST_NAME}.cpp)
    
    target_link_libraries(${TEST_NAME}
        sqlpp_dyn sqlite3
    )
    
    add_test(
        NAME check_${TEST_NAME}
        COMMAND ${TEST_NAME}
    )
endmacro(add_run_test)

add_run_test(basic)
add_run_test(custom_type)
