macro(add_type_test TEST_NAME DEFINITION)
    add_executable(${TEST_NAME} compile/check_types.cpp)
    
    set_target_properties(${TEST_NAME} PROPERTIES
        COMPILE_DEFINITIONS ${DEFINITION}=1
        EXCLUDE_FROM_ALL TRUE
        EXCLUDE_FROM_DEFAULT_BUILD TRUE
    )
    
    target_link_libraries(${TEST_NAME}
        sqlpp_dyn sqlite3
    )
    
    add_test(NAME ${TEST_NAME}
        COMMAND ${CMAKE_COMMAND} --build . --target ${TEST_NAME} --config $<CONFIG>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    set_tests_properties(${TEST_NAME} PROPERTIES
        WILL_FAIL TRUE
    )
endmacro(add_type_test)

add_type_test(check_insert_type CHECK_INSERT_TYPE)
add_type_test(check_insert_count CHECK_INSERT_COUNT)
add_type_test(check_insert_two_tables CHECK_INSERT_TWO_TABLES)
add_type_test(check_select_condition CHECK_SELECT_CONDITION)
