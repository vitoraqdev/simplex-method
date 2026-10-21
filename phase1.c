#include "phase1.h"


#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
Chooses the indices of the basic and non-basic variables for phase I of the simplex method.
The slack variables are added to the basic set, while the original variables are added to the non-basic set.
*/
void initialize_phase1(int m, int n, int *indice_B, int *indice_N) {
    // As variáveis originais (0 até n-1) começam fora da base
    for(int i = 0; i < n; i++){
        indice_N[i] = i;
    }
    for(int i = 0; i < m; i++) {
        indice_B[i] = n + i;
    }
}

/*
Builds the matrices B and N for phase I of the simplex method.
B is the matrix of basic variables (slack variables) (identity matrix)
N is the matrix of non-basic variables (original variables) (A)
*/
void initialization_phase1(double **B_phase1, double **N_phase1, double **A, int rows, int cols) {
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
            N_phase1[i][j] = A[i][j]; // copy A to N
        }
    }

}