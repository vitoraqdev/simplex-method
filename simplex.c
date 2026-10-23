#include "simplex.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>

#include "gaussian_elimination.h"

/*
Source: AlgoritmoPrimalSimplex-MS428.pdf, by Profª Drª Kelly Cristina Poldi
*/

/* Transpose a square matrix of size tam x tam */
int transpose(double **matrix, double **target, const int tam) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            target[j][i] = matrix[i][j];
        }
    }
    return 0;
}

/* Get a column from a matrix */
int get_column(double **matrix, int rows, int column_index, double *column_vector) {
    for (int i = 0; i < rows; i++) {
        column_vector[i] = matrix[i][column_index];
    }

    return 0;
}

/*
Obtains the value of lambda by solving:
B^T * lambda = c_B 
*/
int multiplier_simplex_vector(double **base, double *lambda, double *base_cost, int tam) {
    double **base_transpose = malloc(tam*sizeof(double *));

    for (int i = 0; i < tam; i++) {
        base_transpose[i] = malloc(tam*sizeof(double));
    }

    transpose(base, base_transpose, tam);

    gaussian_elimination(base_transpose, lambda, base_cost, tam);
for (int i = 0; i < tam; i++) {
        free(base_transpose[i]);
    }
    free(base_transpose);
    return 0;
}

/*
Find the reduced cost of each non-basic variable by solving:
ĉ_Nj = c_Nj - λ^T * a_Nj
*/
int find_reduced_cost(double *normal_cost, double *lambda, double **N, int m, int n, double *reduced_cost) {
    for (int i = 0; i < n-m; i++) {
        double dot_product = 0;
        for (int j = 0; j < m; j++) {
            dot_product += lambda[j]*N[j][i];
        }
        reduced_cost[i] = normal_cost[i] - dot_product;
    }

    return 0;
}

/* 
Choose the index of the non-basic variable to enter the basis by finding:
the most negative reduced cost (Dantzig's rule)
We don't want positive relative cost, if all positive return -1
*/
int normal_variable_index_to_enter(double *reduced_cost, int N_cols_size) {
    double best_cost = 0.0;
    int index = -1;

    for (int i = 0; i < N_cols_size; i++) {
        if (reduced_cost[i] < best_cost) {
                best_cost = reduced_cost[i];
                index = i;
            }
        }

    return index;
}

/*
Find the index of the basic variable to exit the basis by finding:
the minimum ratio of current_basic_x[i] / direction[i] for all i where direction[i] > 0
Returns -1 if all directions <= 0 and returns the i-th basic variable
*/
int basic_variable_index_to_exit(double *current_basic_x, double *direction, int tam) {
    double best_epsilon = DBL_MAX;
    int index = -1;

    for (int i = 0; i < tam; i++) {
        if (direction[i] > 0) {
            double current_epsilon = current_basic_x[i]/direction[i];
            if (current_epsilon < best_epsilon) {
                best_epsilon = current_epsilon;
                index = i;
            }
        }
    }

    return index;
}

void generic_swap(void *a, void *b, size_t size) {
    char temp[size];

    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

/*
Swaps between the basic column of B to exit the basis B and
the non-basic column of N to enter the basis B
*/
int switch_basic_and_normal_column(double **B, double **N, int base_col_index, int normal_col_index, int rows) {
    for (int i = 0; i < rows; i++) {
        generic_swap(&B[i][base_col_index], &N[i][normal_col_index], sizeof(double));
    }

    return 0;
}



/*
Columns refer to the A_mxn columns, N has (cols-rows) columns
each iteration will switch the columns as needed,
while tracking in basic_index and normal_index

Returns are:
-1 for any fails;
0 for keep going;1 for optimality.
*/
int simplex_step(double **B, double **N, double *b, double *basic_cost, double *normal_cost, int *basic_index,
    int *normal_index, int rows, int cols, int *iteration) {
    SimplexStatus STATUS = SIMPLEX_MEMORY_FAILURE; // assume error

    double *current_basic_x = malloc(rows*sizeof(double));
    double *lambda = malloc(rows*sizeof(double));
    double **B_transpose = calloc(rows, sizeof(double *));
    double *reduced_cost = malloc((cols-rows)*sizeof(double));
    double *N_col = malloc(rows*sizeof(double));
    double *direction = malloc(rows*sizeof(double));

    if (!current_basic_x || !lambda || !B_transpose || !reduced_cost || !N_col || !direction) {
        STATUS = SIMPLEX_MEMORY_FAILURE;
        goto cleanup;
    }

    gaussian_elimination(B, current_basic_x, b, rows);

    for (int i = 0; i < rows; i++) {
        B_transpose[i] = malloc(rows * sizeof(double));
        if (!B_transpose[i]) {
            STATUS = SIMPLEX_MEMORY_FAILURE;
            goto cleanup;
        }
    }
    transpose(B, B_transpose, rows);

    gaussian_elimination(B_transpose, lambda, basic_cost, rows); //Finds the value of lambda by solving B^T * lambda = c_B

    find_reduced_cost(normal_cost, lambda, N, rows, cols, reduced_cost);

    int normal_variable_index = normal_variable_index_to_enter(reduced_cost, cols-rows);

    if (normal_variable_index == -1) {
        STATUS = SIMPLEX_OPTIMAL;
        goto cleanup;
    }

    /*Takes the column of the non-basic variable to enter the basis*/
    get_column(N, rows, normal_variable_index, N_col);

    /*
    Find the direction of the next vertex by solving:
    B * direction = a_Nk
    */
    gaussian_elimination(B, direction, N_col, rows);

    int basic_variable_index = basic_variable_index_to_exit(current_basic_x, direction, rows);
    if (basic_variable_index == -1){
        STATUS = SIMPLEX_NO_FINITE_SOLUTION;
        goto cleanup;
    }

    switch_basic_and_normal_column(B, N, basic_variable_index, normal_variable_index, rows);

    generic_swap(&basic_cost[basic_variable_index], &normal_cost[normal_variable_index], sizeof(double));

    generic_swap(&basic_index[basic_variable_index], &normal_index[normal_variable_index], sizeof(int));

    STATUS = SIMPLEX_KEEP_GOING;
    (*iteration)++;

cleanup:
    if (B_transpose) {
        for (int i = 0; i < rows; i++) {
            free(B_transpose[i]);
        }
        free(B_transpose);
    }
    free(current_basic_x);
    free(lambda);
    free(reduced_cost);
    free(N_col);
    free(direction);

    return STATUS;
}