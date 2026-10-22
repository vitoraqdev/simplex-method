cmake_minimum_required(VERSION 4.3)
project(simplex_method C)

set(CMAKE_C_STANDARD 11)

add_executable(SimplexMethod
        main.c
        simplex.c
        simplex.h
        gaussian_elimination.c
        gaussian_elimination.h
        auxiliar_functions.c
        auxiliar_functions.h
        initialize.c
        initialize.h
        phases_1_and_2.c
        phases_1_and_2.h
        )

add_executable(SimplexTests
        test.c
        test.h
        simplex.c
        simplex.h
        gaussian_elimination.c
        gaussian_elimination.h)