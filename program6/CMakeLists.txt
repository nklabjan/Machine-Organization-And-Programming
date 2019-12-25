cmake_minimum_required(VERSION 3.8)
project(program6)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES sendsig.c division.c)
add_executable(program6 ${SOURCE_FILES})