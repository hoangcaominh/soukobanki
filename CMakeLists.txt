cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(soukobanki)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(vendor)

file(GLOB SOURCES_H RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*/*.h)
file(GLOB SOURCES_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*/*.cpp)

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_sources(
    ${PROJECT_NAME}
    PRIVATE
    src/main.cpp
    ${SOURCES_H}
    ${SOURCES_CPP}
)

target_link_libraries(${PROJECT_NAME} PRIVATE vendor)
