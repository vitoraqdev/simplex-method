#include "initialize.h"

#include <stdio.h>
#include <stdlib.h>

int initialize_input(double **c, double ***A, double **b, int *m, int *n){
    FILE *input = fopen("./input.txt", "r");

    if (input == NULL) {
        perror("Error reading input.txt");
        return -1;
    }
    
    if (fscanf(input, "%d %d", m, n) != 2){
        fclose(input);
        return -1;
    }

    *c = malloc(*n * sizeof(double));
    if (!*c) 
        goto cleanup;

    for (int j = 0; j < *n; j++) {
        fscanf(input, "%lf", &(*c)[j]);
    }


    *A = malloc(*m * sizeof(double *));
    if (!*A)
        goto cleanup;

    for (int i = 0; i < *m; i++) {
        (*A)[i] = malloc(*n * sizeof(double));
        if (!(*A)[i]) 
            goto cleanup;
        for (int j = 0; j < *n; j++) {
            fscanf(input, "%lf", &(*A)[i][j]);
        }
    }


    *b = malloc(*m * sizeof(double));
    if (!*b)
        goto cleanup;
    for (int i = 0; i < *m; i++) {
        fscanf(input, "%lf", &(*b)[i]);
    }

    fclose(input);
    return 0;

    cleanup:
        if (*c) { free(*c); *c = NULL; }
        if (*b) { free(*b); *b = NULL; }
        if (*A) {
            for (int i = 0; i < *m; i++) {
                if ((*A)[i]) free((*A)[i]);
            }
            free(*A);
            *A = NULL;
        }

    fclose(input);
    return -1;
}