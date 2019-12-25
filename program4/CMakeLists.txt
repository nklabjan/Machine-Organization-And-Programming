cmake_minimum_required(VERSION 3.8)
project(program4)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES cache1D.c cache2Drows.c cache2Dcols.c csim.c)
add_executable(program4 ${SOURCE_FILES})