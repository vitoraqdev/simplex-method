cmake_minimum_required(VERSION 4.3)
project(simplex_method C)

set(CMAKE_C_STANDARD 11)

add_executable(SimplexMethod
        main.c
        simplex.c
        simplex.h)

add_executable(SimplexTests
        test.c
        test.h
        simplex.c
        simplex.h)