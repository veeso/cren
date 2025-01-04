cmake_minimum_required(VERSION 3.15)

project(cren LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib
)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(cren ${SOURCES})

target_compile_options(cren PRIVATE -Wall -Wextra -pedantic)
