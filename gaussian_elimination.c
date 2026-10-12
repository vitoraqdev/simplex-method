//
// Created by vitor on 11/10/2026.
//

#include "gaussian_elimination.h"

#include <stdlib.h>
#include <math.h>

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
int gaussian_elimination(double **A, double *x, double *b, int n) {
    double *s = malloc(n*sizeof(double)); // row maximal elements
    int *p = malloc(n*sizeof(int)); // row pointer

    for (int i = 0; i < n; i++) {
        s[i] = 0;
        for (int j = 0; j < n; j++) {
            if (s[i] < fabs(A[i][j])) // fabs é absolute pra double
                s[i] = fabs(A[i][j]);
            p[i] = i;
        }
    }

    int j;
    for (int k = 0; k < n-1; k++) {
        double r_max = 0;
        for (int i = k; i < n; i++) {
            double r = fabs(A[p[i]][k] / s[p[i]]);
            if (r > r_max) {
                r_max = r;
                j = i;
            }
        }

        int temp = p[k];
        p[k] = p[j];
        p[j] = temp;

        for (int i = k+1; i < n; i++) {
            A[p[i]][k] /= A[p[k]][k];
            for (int j = k+1; j < n; j++) {
                A[p[i]][j] -= A[p[i]][k] * A[p[k]][j];
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
        double s = b[p[i]];
        for (int j = i+1; j < n; j++) {
            s -= A[p[i]][j]*x[j];
        }
        x[i] = s/A[p[i]][i];
    }

    free(p);
    free(s);
    return 0;

}

