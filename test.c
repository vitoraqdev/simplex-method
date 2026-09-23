#include "test.h"
#include "simplex.h"


// --- TESTES UNITÁRIOS ---
void test_soma(void) {
    assert_equal(5, soma(2, 3));
    assert_equal(0, soma(-2, 2));
}

// --- MAIN QUE RODA OS TESTES ---
int main(void) {
    printf("=== EXECUTANDO TESTES UNITARIOS ===\n\n");

    test_soma();

    printf("\n-----------------------------------\n");
    printf("Resultado: %d/%d testes passaram.\n", tests_run - tests_failed, tests_run);

    return (tests_failed > 0) ? 1 : 0;
}