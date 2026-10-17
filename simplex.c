#include "simplex.h"
#include <stdlib.h>

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
int reduced_cost(double *normal_cost, double *lambda, double **N, int m, int n, double *reduced_cost) {
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
