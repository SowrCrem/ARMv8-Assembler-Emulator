cmake_minimum_required(VERSION 3.23)
project(armv8_47 C)

set(CMAKE_C_STANDARD 11)

add_executable(armv8_47
        src/assemble.c
        src/emulate.c)
