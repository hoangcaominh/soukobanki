cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(soukobanki)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(vendor)
add_subdirectory(src)
