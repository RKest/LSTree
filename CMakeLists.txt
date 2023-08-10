cmake_minimum_required(VERSION 3.23)

project(LSTree)

include(cmake/find_libs.cmake)
include(GoogleTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE ls_tree)