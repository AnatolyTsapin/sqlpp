cmake_minimum_required(VERSION 3.16.3)

project("SQLpp with tests")

enable_testing()

add_subdirectory(./source)
add_subdirectory(./tests)

file(GLOB_RECURSE CPP_FILES *.cpp *.h)
add_custom_target(
    format
    COMMAND
    clang-format -i ${CPP_FILES}
)
