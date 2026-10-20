#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gaussian_elimination.h"
#include "simplex.h"

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

    return 0;
}
// Vou implementar a fase I aqui, guardando quem está na base (B) e quem está fora (N)
void inicializador_fase1(int m, int n, int *indice_B, int *indice_N) {
    // As variáveis originais (0 até n-1) começam fora da base
    for(int i = 0; i < n; i++){
        indice_N[i] = i;
    }
    for(int i = 0; i < m; i++) {
        indice_B[i] = n + i;
    }
}
// Constrói a matriz inicial da Fase I juntando a Matriz A com a Matriz Identidade
// void montar_quadro(int m, int n, double **quadro, double **A) {
//     for(int i = 0; i < m; i++){
//         for(int j = 0; j < (n + m); j++) {
//             if(j < n){
//                 // Copia os coeficientes originais e converte de inteiro para decimal
//                 quadro[i][j] = A[i][j]; // Converte o int dele para o teu float
//             } else if(j == n + i){
//                 // 1.0 na diagonal da área das variáveis artificiais (Matriz Identidade)
//                 quadro[i][j] = 1.0;
//             } else {
//                 quadro[i][j] = 0.0;
//             }
//         }
//     }
// }



int initialization_phase1(double **B_phase1, double **N_phase1, double **A, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        B_phase1[i] = malloc(rows*sizeof(double));
        N_phase1[i] = malloc(cols*sizeof(double));
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            if (i == j)
                B_phase1[i][j] = 1; // identity
            else
                B_phase1[i][j] = 0;
        }

        for (int j = 0; j < cols; j++) {
            N_phase1[i][j] = A[i][j]; // memcpy
        }
    }

}



int is_in_index(int *index, int length, int variable) {
    int is_inside = 0;
    for (int i = 0; i < length; i++) {
        if (index[i] == variable)
            is_inside = 1;
    }

    return is_inside;
}

// e.g. index_B = [2, 3], meaning first basic variable is the second original variable
// we want to find
int inverse_index(int *index, int length, int variable) {
    if (!is_in_index(index, length, variable))
        return -1;

    for (int i = 0; i < length; i++) {
        if (index[i] == variable)
            return i;
    }
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
    int max_iteration = 100;
    double tolerance = 1e-6;

    inicializar_input(&c, &A, &b, &m, &n);


    // Alocação de memória para o quadro (m linhas por n+m colunas)
    double **quadro = malloc(m * sizeof(double *));
    for (int i = 0; i < m; i++) {
        quadro[i] = malloc((n + m) * sizeof(double));
    }
    // Alocação dos vetores de índices Básicos (B) e Não-Básicos (N)
    int *B_index_phase1 = malloc(m * sizeof(int));
    int *N_index_phase1 = malloc(n * sizeof(int));

    // Chama as funções para preencher os índices e montar a matriz
    inicializador_fase1(m, n, B_index_phase1, N_index_phase1);
    // montar_quadro(m, n, quadro, A);

    double **B_phase1 = calloc(m, sizeof(double *));;
    double **N_phase1 = calloc(m, sizeof(double *));;
    initialization_phase1(B_phase1, N_phase1, A, m, n);

    double *basic_cost_phase1 = malloc(m*sizeof(double));
    double *non_basic_cost_phase1 = malloc(n*sizeof(double));
    for (int i = 0; i < m; i++) {
        basic_cost_phase1[i] = 1;
    }
    for (int i = 0; i < n; i++) {
        non_basic_cost_phase1[i] = 0;
    }

    // for (int i = 0; i < m; i++) {
    //     for (int j = 0; j < m; j++) {
    //         printf("%.1lf ", B_phase1[i][j]); // just to check, delete later
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    //
    // for (int i = 0; i < m; i++) {
    //     for (int j = 0; j < n; j++) {
    //         printf("%.0lf ", N_phase1[i][j]); // just to check, delete later
    //     }
    //     printf("\n");
    // }
    // printf("\n");

    double *current_basic_x = malloc(m*sizeof(double));
    int iteration = 0;
    // int status = simplex_step(B_phase1, N_phase1, b, basic_cost_phase1, normal_cost_phase1,
        // indice_B, indice_N, m, m+n, &iteration);

    int phase1_status = 0;
    while (phase1_status == 0 && iteration <= max_iteration) {
        phase1_status = simplex_step(B_phase1, N_phase1, b, basic_cost_phase1, non_basic_cost_phase1,
        B_index_phase1, N_index_phase1, m, m+n, &iteration);
    }

    if (phase1_status == -2) {
        printf("ERROR: No finite solution for the first phase.");
        goto cleanup;
    } else if (phase1_status == -1) {
        printf("ERROR: Not enough memory to allocate.");
        goto cleanup;
    } else if (phase1_status == 0) {
        printf("ERROR: Max iteration (%d) reached.", max_iteration);
        goto cleanup;
    }

    // if finds optimal solution check if cost is zero
    double total_cost = 0;
    gaussian_elimination(B_phase1, current_basic_x, b, m); // redefine x
    for (int i = 0; i < m; i++) {
        total_cost += basic_cost_phase1[i]*current_basic_x[i];
    }


    if (fabs(total_cost) > tolerance) { // if total_cost > 0
        printf("ERROR: First phase failed. Total cost %.6lf != 0", total_cost);
        goto cleanup;
    }

    // else, total_cost = 0
    // here we have a problem.
    // either the artificial variable is basic, or not
    // if is basic, then i kinda don't know what to do, gemini says to check i-th row of B^-1*N
    // select non basic variable xj to enter its place if the (i, j) coordinate of B^-1*N is non zero.
    // but we can't calculate B^-1 accurately. and in slides and books, i did not find what to do in this case
    // maybe solving the dual is better in this case.
    // we could also add dual if it would be faster (n > m or something)
    // anyways i'm assuming artificial is non-basic.

    int *B_index_phase2 = B_index_phase1;
    int *N_index_phase2 = malloc((n-m) * sizeof(int));
    double **B_phase2 = B_phase1;
    double **N_phase2 = malloc(m * sizeof(double *));
    for (int i = 0; i < m; i++) {
        N_phase2[i] = malloc((n-m) * sizeof(double));
    }


    // print x_0, ..., x_(m+n-1)
    // n variables should be 0
    for (int i = 0; i < m+n; i++) {
        if (is_in_index(B_index_phase1, m, i)) {
            int inv_index = inverse_index(B_index_phase1, m, i); // where it is in the B_index
            printf("x_%d: %.6lf\n", i, current_basic_x[inv_index]);
        } else
            printf("x_%d: 0\n", i);
    }
    printf("\n");


    // filter out artificial variables
    int n_count = 0;
    for (int j = 0; j < n; j++) {
        if (!is_in_index(B_index_phase2, m, j)) { // if j-th variable is not basic
            N_index_phase2[n_count] = j; // add that to the next available slot

            // copy column
            for (int i = 0; i < m; i++) {
                N_phase2[i][n_count] = A[i][j]; // next available column copies the j-th original column
            }
            n_count++;
        }
    }


    double *basic_cost_phase2 = malloc(m * sizeof(double));
    double *non_basic_cost_phase2 = malloc((n - m) * sizeof(double));

    for (int i = 0; i < m; i++) {
        int ith_basic_variable_position = B_index_phase2[i];
        basic_cost_phase2[i] = c[ith_basic_variable_position];
    }

    for (int i = 0; i < n-m; i++) {
        int ith_non_basic_variable_position = N_index_phase2[i];
        non_basic_cost_phase2[i] = c[ith_non_basic_variable_position];
    }


    // now, using the new B we run the actual simplex method, phase 2
    int phase2_status = 0;
    iteration = 0;
    while (phase2_status == 0 && iteration <= max_iteration) {
        phase2_status = simplex_step(B_phase2, N_phase2, b, basic_cost_phase2, non_basic_cost_phase2,
        B_index_phase2, N_index_phase2, m, n, &iteration);
    }

    if (phase1_status == -2) {
        printf("ERROR: No finite solution for the first phase.");
        goto cleanup;
    }
    if (phase1_status == -1) {
        printf("ERROR: Not enough memory to allocate.");
        goto cleanup;
    }
    if (phase1_status == 0) {
        printf("ERROR: Max iteration (%d) reached.", max_iteration);
        goto cleanup;
    }

    // finds optimal value of
    total_cost = 0;
    gaussian_elimination(B_phase2, current_basic_x, b, m); // redefine x
    for (int i = 0; i < m; i++) {
        total_cost += basic_cost_phase2[i]*current_basic_x[i];
    }


    // print x_0, ..., x_(n-1)
    // n variables should be 0
    for (int i = 0; i < n; i++) {
        if (is_in_index(B_index_phase2, m, i)) {
            int inv_index = inverse_index(B_index_phase2, m, i); // where it is in the B_index
            printf("x_%d: %.6lf\n", i, current_basic_x[inv_index]);
        } else
            printf("x_%d: 0\n", i);
    }



cleanup:
    for (int i = 0; i < m; i++) {
        free(A[i]);
        free(B_phase1[i]);
        free(N_phase1[i]);
    }
    free(c);
    free(A);
    free(b);
    free(B_phase1);
    free(N_phase1);
    free(quadro);
    free(B_index_phase1);
    free(N_index_phase1);
    // free(custos_fase1);
    free(basic_cost_phase1);
    free(non_basic_cost_phase1);
    liberar_memoria(c, A, b, m);
    return 0;

}
