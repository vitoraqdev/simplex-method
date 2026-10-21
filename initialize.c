#include "initialize.h"

#include <stdio.h>
#include <stdlib.h>

int initialize_input(double **c, double ***A, double **b, int *m, int *n) {
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