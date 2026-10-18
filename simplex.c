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
