cmake_minimum_required(VERSION 3.15)

project(cren LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(cren ${SOURCES})

target_compile_options(cren PRIVATE -Wall -pedantic)


# link pthread
find_package(Threads REQUIRED)
target_link_libraries(cren PRIVATE Threads::Threads)
