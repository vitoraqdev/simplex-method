#include "gaussian_elimination.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

/*
Does the gaussian elimination method to solve the system of equations Ax = b, where A is a n x n matrix and b is a vector.
Howewever, the basis matrix is always square, so we can assume that A is always square and b is always a vector of size n.
The solution will be stored in x, which is also a vector of size n.

Algorithm pseudocode source: https://math.gordon.edu/courses/mat342/handouts/gauss.pdf
*/

#define EPSILON 1e-6

int gaussian_elimination(double **A, double *x, double *b, int n) {
    /*
    Copies matrices A and b, so we don't allocchange the original ones.
    x is not copied, because we want to change it and return the solution.
    
    malloc initializes all rows as NULL, making it safe to clean if something goes wrong during the rows malloc
    */
    double **A_copy = calloc(n, sizeof(double *));
    double *b_copy = malloc(n * sizeof(double));
    double *s = malloc(n*sizeof(double)); //Row maximal elements
    int *p = malloc(n*sizeof(int)); //Row pointer

    /*
    We assume failure, and set status to 0 if the algorithm succeeds. This way, we can use goto cleanup to free memory and return failure if something goes wrong.
    */
    int status = -1;

    if (!A_copy || !b_copy || !s || !p) {
        goto cleanup;
    }

    for (int i = 0; i < n; i++) {
        A_copy[i] = malloc(n * sizeof(double));
        if (!A_copy[i]) {
            goto cleanup;
        }
        b_copy[i] = b[i];
        for (int j = 0; j < n; j++) {
            A_copy[i][j] = A[i][j];
        }
    }

    for (int i = 0; i < n; i++) {
        s[i] = 0;
        p[i] = i;
        for (int j = 0; j < n; j++) {
            if (s[i] < fabs(A_copy[i][j]))
                s[i] = fabs(A_copy[i][j]);
        }
    }

    int j;
    for (int k = 0; k < n-1; k++) {
        double r_max = 0;
        j = k;
        for (int i = k; i < n; i++) {
            if (fabs(s[p[i]]) > EPSILON ){
                double r = fabs(A_copy[p[i]][k] / s[p[i]]);
                if (r > r_max) {
                    r_max = r;
                    j = i;
                }
            } else{
                printf("ERROR: Singular Matrix\n");
                goto cleanup;
            }
        }

        int temp = p[k];
        p[k] = p[j];
        p[j] = temp;

        /*
        Realizes the elimination by subtracting the k-th row from the i-th row in A, multiplied by the factor A[i][k]/A[k][k]
        */
        for (int i = k+1; i < n; i++) {
            if (fabs(A_copy[p[k]][k]) > EPSILON){
                double multi;
                multi = A_copy[p[i]][k] / A_copy[p[k]][k];
                A_copy[p[i]][k] = multi;
                for (int j = k+1; j < n; j++) {
                    A_copy[p[i]][j] -= multi * A_copy[p[k]][j];
                }
            } else{
                printf("ERROR: Singular Matrix\n");
                goto cleanup;
            }
        }

    }

    /*
    Realizes the elimination by subtracting the k-th row from the i-th row in b, multiplied by the factor A[i][k]/A[k][k]
    */
    for (int k = 0; k < n-1; k++) {
        for (int i = k+1; i < n; i++) {
            b_copy[p[i]] -= A_copy[p[i]][k]*b_copy[p[k]];
        }
    }

    /*
    Finds the solution by back substitution, starting from the last row and going up to the first row.
    */
    for (int i = n-1; i >= 0; i--) {
        double v = b_copy[p[i]];
        for (int j = i+1; j < n; j++) {
            v -= A_copy[p[i]][j]*x[j];
        }
        if (fabs(A_copy[p[i]][i])>EPSILON){
            x[i] = v/A_copy[p[i]][i];
        } else{
            printf("ERROR: Singular Matrix\n");
            goto cleanup;
        }
    }

    status = 0;

cleanup:
    if (A_copy) {
        for (int i = 0; i < n; i++) {
            free(A_copy[i]); // O free(NULL) é seguro em C, mas só liberamos o que foi alocado
        }
        free(A_copy);
    }
    free(b_copy);
    free(p);
    free(s);

    return status;
}