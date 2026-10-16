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

    return 0;
}
