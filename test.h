// MACRO PARA UNIT-TESTS
#ifndef SIMPLEX_METHOD_TEST_H
#define SIMPLEX_METHOD_TEST_H

#include <stdio.h>

int tests_run = 0;
int tests_failed = 0;

#define assert_equal(expected, actual) do { \
    tests_run++; \
    if ((expected) != (actual)) { \
        printf("[ERRO] %s:%d | Esperado: %d, Recebido: %d\n", \
               __FILE__, __LINE__, (expected), (actual)); \
        tests_failed++; \
    } else { \
        printf("[OK] %s:%d\n", __FILE__, __LINE__); \
    } \
} while(0)

#endif //SIMPLEX_METHOD_TEST_H
