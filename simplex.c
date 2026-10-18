#include "simplex.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>

#include "gaussian_elimination.h"


// transpose of matrix nxn
int transpose(double **matrix, double **target, const int tam) {
    // send (i, j) from matrix to (j, i) of target
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            target[j][i] = matrix[i][j];
        }
    }
    return 0;
}

int get_column(double **matrix, int rows, int column_index, double *column_vector) {
    for (int i = 0; i < rows; i++) {
        column_vector[i] = matrix[i][column_index];
    }

    return 0;
}

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

// last variable is the one we're defining
int find_reduced_cost(double *normal_cost, double *lambda, double **N, int m, int n, double *reduced_cost) {
    for (int i = 0; i < n-m; i++) {
        // reduced_cost_j = c_N_j - lambda^T * a^N_j
        double dot_product = 0;
        for (int j = 0; j < m; j++) {
            dot_product += lambda[j]*N[j][i]; // multiply row vector x column vector (dot product)
        }
        reduced_cost[i] = normal_cost[i] - dot_product;
    }

    return 0;
}

int find_direction(double **B, double *direction, int tam, double *A_column) {
    gaussian_elimination(B, direction, A_column, tam);
}


int normal_variable_index_to_enter(double *reduced_cost, int N_cols_size) {
    double best_cost = DBL_MAX;
    int index = -1;

    for (int i = 1; i < N_cols_size; i++) {
        if (reduced_cost[i] < 0) { // we don't want positive relative cost, if all positive return -1
            if (reduced_cost[i] < best_cost) {
                best_cost = reduced_cost[i];
                index = i;
            }
        }
    }

    return index;
}

// returns -1 if all directions <= 0
// and returns the i-th basic variable
int basic_variable_index_to_exit(double *current_basic_x, double *direction, int tam) {
    double best_epsilon = DBL_MAX;
    int index = -1; // -1 means direction <= 0

    for (int i = 1; i < tam; i++) {
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

double find_step_size(double *current_basic_x, double *direction, int index) {
    return current_basic_x[index]/direction[index];
}

void generic_swap(void *a, void *b, size_t size) {
    // Allocate a temporary buffer on the stack using a VLA (Variable Length Array)
    char temp[size];

    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

// switches the base_col_index-th base column with normal_col_index-th normal column
int switch_basic_and_normal_column(double **B, double **N, int base_col_index, int normal_col_index, int rows) {
    for (int i = 0; i < rows; i++) {
        generic_swap(&B[i][base_col_index], &N[i][normal_col_index], sizeof(double));
    }

    return 0;
}


typedef enum {
    SIMPLEX_NO_FINITE_SOLUTION = -2,
    SIMPLEX_MEMORY_FAILURE = -1,
    SIMPLEX_KEEP_GOING = 0,
    SIMPLEX_OPTIMAL = 1
} SimplexStatus;


// columns refer to the A_mxn columns, N has (cols-rows) columns
// each iteration it will switch the columns as needed, while tracking in basic_index and normal_index
// e.g. basic_index = [1, 2, 4] means that the original x1, x2 and x4 are basic
// return are -1 for any fails, 0 for keep going and 1 for optimality
// AlgoritmoPrimalSimplex-MS428.pdf
int simplex_step(double **B, double **N, double *b, double *basic_cost, double *normal_cost, int *basic_index,
    int *normal_index, int rows, int cols, int *iteration) {
    SimplexStatus STATUS = SIMPLEX_MEMORY_FAILURE; // assume error

    double *current_basic_x = malloc(rows*sizeof(double));
    double *lambda = malloc(cols*sizeof(double));
    double **B_transpose = calloc(rows, sizeof(double *));
    double *reduced_cost = malloc((cols-rows)*sizeof(double));
    double *N_col = malloc(rows*sizeof(double));
    double *direction = malloc(rows*sizeof(double));


    gaussian_elimination(B, current_basic_x, b, rows); // defines current_basic_x

    for (int i = 0; i < rows; i++) {
        B_transpose[i] = malloc(rows * sizeof(double));
    }
    transpose(B, B_transpose, rows);
    gaussian_elimination(B_transpose, lambda, basic_cost, rows); // defines lambda

    find_reduced_cost(normal_cost, lambda, N, rows, cols, reduced_cost); // defines relative_cost

    int normal_variable_index = normal_variable_index_to_enter(reduced_cost, cols-rows);

    if (normal_variable_index == -1) {
        STATUS = SIMPLEX_OPTIMAL;
        goto cleanup;
    }

    get_column(N, rows, normal_variable_index, N_col); // define N_col

    gaussian_elimination(B, direction, N_col, rows); // define direction

    // if y <= 0
    int FLAG = -1;
    for (int i = 0; i < rows; i++) {
        if (direction[i] > 0) {
            FLAG = 0;
        }
    }

    if (FLAG == -1) {
        STATUS = SIMPLEX_NO_FINITE_SOLUTION;
        goto cleanup;
    }

    int basic_variable_index = basic_variable_index_to_exit(current_basic_x, direction, rows);
    double step_size = find_step_size(current_basic_x, direction, basic_variable_index); // CAN REMOVE?

    // switch B and N
    switch_basic_and_normal_column(B, N, basic_variable_index, normal_variable_index, rows);

    // switch c_B and c_N
    generic_swap(&basic_cost[basic_variable_index], &normal_cost[normal_variable_index], sizeof(double));

    // switch basic_index and normal_index
    generic_swap(&basic_index[basic_variable_index], &normal_index[normal_variable_index], sizeof(int));

    STATUS = SIMPLEX_KEEP_GOING; // keep going iteration
    (*iteration)++;

cleanup:
    for (int i = 0; i < rows; i++) {
        free(B_transpose[i]);
    }
    free(current_basic_x);
    free(lambda);
    free(B_transpose);
    free(reduced_cost);
    free(N_col);
    free(direction);

    return STATUS;
}

