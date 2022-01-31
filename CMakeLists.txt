cmake_minimum_required(VERSION 3.21)
project(japanese_uno_bit)

set(CMAKE_CXX_STANDARD 14)

add_executable(japanese_uno_bit main.cpp)

MATH(EXPR stack_size "4096 * 1024 * 1024")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,${stack_size}")