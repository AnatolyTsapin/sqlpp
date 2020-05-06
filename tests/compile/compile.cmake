macro(add_type_test TEST_NAME DEFINITION FAIL)
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
    
    if(${FAIL})
	    set_tests_properties(${TEST_NAME} PROPERTIES
	        WILL_FAIL TRUE
	    )
    endif()
endmacro(add_type_test)

add_type_test(check_insert_type_pass CHECK_INSERT_TYPE_PASS FALSE)
add_type_test(check_insert_type_fail CHECK_INSERT_TYPE_FAIL TRUE)
add_type_test(check_insert_count_fail CHECK_INSERT_COUNT_FAIL TRUE)
add_type_test(check_insert_values_pass CHECK_INSERT_VALUES_PASS FALSE)
add_type_test(check_insert_values_twice_fail CHECK_INSERT_VALUES_TWICE_FAIL TRUE)
add_type_test(check_insert_two_tables_fail CHECK_INSERT_TWO_TABLES_FAIL TRUE)
add_type_test(check_select_condition_pass CHECK_SELECT_CONDITION_PASS TRUE)
add_type_test(check_select_condition_fail CHECK_SELECT_CONDITION_FAIL TRUE)
