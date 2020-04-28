cmake_minimum_required(VERSION 3.16.3)

project("SQLpp with tests")

enable_testing()

add_subdirectory(./source)
add_subdirectory(./tests)
