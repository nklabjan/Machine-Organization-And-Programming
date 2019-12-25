cmake_minimum_required(VERSION 3.8)
project(program3test)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES mem.c)
add_executable(program3test ${SOURCE_FILES})