cmake_minimum_required(VERSION 3.8)
project(program2)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES verify_hetero.c)
add_executable(program2 ${SOURCE_FILES})