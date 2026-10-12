#include <stdio.h>
#include <stdlib.h>

// TODO: escrever headers, código e testes de
// 1. resolvedor de sistema Ax=b
// 2. cálculo de custos relativos
// 3. teste de otimalidade
// 4. direção simplex
// 5. tamanho do passo
// 6. nova partição básica

// os testes é fácil escrever, basta pegar um exemplo resolvido
// e pegar as entradas e saídas
// mas para isso precisa ter o header bonitinho.

int inicializar_input(double **c, double ***A, double **b, int *m, int *n) {
    FILE *input = fopen("./input.txt", "r");

    if (input == NULL) {
        perror("Error reading input.txt");
    }
    fscanf(input, "%d %d", m, n);


    *c = malloc(*n * sizeof(double));
    for (int j = 0; j < *n; j++) {
        fscanf(input, "%lf", &(*c)[j]);
    }


    *A = malloc(*m * sizeof(double *));
    for (int i = 0; i < *m; i++) {
        (*A)[i] = malloc(*n * sizeof(double));
        for (int j = 0; j < *n; j++) {
            fscanf(input, "%lf", &(*A)[i][j]);
        }
    }


    *b = malloc(*m * sizeof(double));
    for (int i = 0; i < *m; i++) {
        fscanf(input, "%lf", &(*b)[i]);
    }


    fclose(input);
    return 0;
}

int liberar_memoria(double *c, double **A, double *b, int m) {
    free(c);
    for (int i = 0; i < m; i++) {
        free(A[i]);
    }
    free(A);
    free(b);
    return 0;
}


int main() {
    // a entrada será feita pelo arquivo input.txt
    // 1° entrada: 1 linha dizendo qual tamanho mxn da matriz
    // m são as restrições
    // n são a quantidade de variáveis
    // 2° entrada: 1 linha da matriz c^T
    // 3° entrada: a matriz A linha a linha
    // 4° entrada: 1 linha para a matriz b^T

    double *c, *b;
    double **A;
    int m, n;

    inicializar_input(&c, &A, &b, &m, &n);

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%lf ", A[i][j]); // just to check, delete later
        }
        printf("\n");
    }


    liberar_memoria(c, A, b, m);
    return 0;

}