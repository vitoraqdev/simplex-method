//
// Created by vitor on 11/10/2026.
// Modified by Thierry Scarazzatti Adame on 16/10/2026.
//

#include "gaussian_elimination.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

// can delete, not using for new gaussian elimination
// int max_from_column(double **A, int m, int n, int k) {
//     double highest = A[0][k];
//     int index = 0;
//
//     for (int i = 1; i < m; i++) {
//         if (A[i][k] > highest) {
//             highest = A[i][k];
//             index = i;
//         }
//     }
//
//     return index;
// }

// can delete, not using for new gaussian elimination
// void swap_rows(double **A, int m, int n, int h, int i) {
//     int size_of_bytes_to_copy = n*sizeof(double);
//     int *aux_row_vector = malloc(size_of_bytes_to_copy);
//
//     memcpy(aux_row_vector, A[h], size_of_bytes_to_copy);
//     memcpy(A[h], A[i], size_of_bytes_to_copy);
//     memcpy(A[i], aux_row_vector, size_of_bytes_to_copy);
// }

// work only for matrix A nxn, since otherwise it'd have variables, which we don't want
// also, from revised simplex, B is nxn and also a basis, so is no problem and always will have solution.
// https://math.gordon.edu/courses/mat342/handouts/gauss.pdf

#define EPSILON 1e-12

int gaussian_elimination(double **A, double *x, double *b, int n) {
    double *s = malloc(n*sizeof(double)); // row maximal elements
    int *p = malloc(n*sizeof(int)); // row pointer
    if((s == NULL)||(p == NULL)){
            printf("ERROR: Out of Memory\n");
            free(p);
            free(s);
            return -1;
    }

    for (int i = 0; i < n; i++) {
        s[i] = 0;
        p[i] = i;
        for (int j = 0; j < n; j++) {
            if (s[i] < fabs(A[i][j])) // fabs é absolute pra double
                s[i] = fabs(A[i][j]);
        }
    }

    int j;
    for (int k = 0; k < n-1; k++) {
        double r_max = 0;
        j = k;
        for (int i = k; i < n; i++) {
            if (fabs(s[p[i]]) > EPSILON ){
                double r = fabs(A[p[i]][k] / s[p[i]]);
                if (r > r_max) {
                    r_max = r;
                    j = i;
                }
            } else{
                printf("ERROR: Singular Matrix\n");
                free(p);
                free(s);
                return -1;
            }
        }

        int temp = p[k];
        p[k] = p[j];
        p[j] = temp;

        for (int i = k+1; i < n; i++) {
            if (fabs(A[p[k]][k])> EPSILON){
                double multi;
                multi = A[p[i]][k] / A[p[k]][k];
                A[p[i]][k] = multi;
                for (int j = k+1; j < n; j++) {
                    A[p[i]][j] -= multi * A[p[k]][j];
                }
            } else{
                printf("ERROR: Singular Matrix\n");
                free(p);
                free(s);
                return -1;
            }
        }

    }

    // Forward elimination
    for (int k = 0; k < n-1; k++) {
        for (int i = k+1; i < n; i++) {
            b[p[i]] -= A[p[i]][k]*b[p[k]];
        }
    }

    // Backwards solve
    for (int i = n-1; i >= 0; i--) {
        double v = b[p[i]];
        for (int j = i+1; j < n; j++) {
            v -= A[p[i]][j]*x[j];
        }
        if (fabs(A[p[i]][i])>EPSILON){
            x[i] = v/A[p[i]][i];
        } else{
            printf("ERROR: Singular Matrix\n");
            free(p);
            free(s);
            return -1;
        }
    }

    free(p);
    free(s);
    return 0;

}