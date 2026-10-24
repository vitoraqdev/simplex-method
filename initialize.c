#include "initialize.h"

#include <stdio.h>
#include <stdlib.h>

int initialize_input(double **c, double ***A, double **b, double **optimal_solution, int *m, int *n){
    *c = NULL;
    *A = NULL;
    *b = NULL;
    *optimal_solution = NULL;

    /*Reading m (lines/constrains)*/
    FILE *input_m = fopen("lp_test_m.bin", "rb");
    if (input_m == NULL) {
        perror("Error reading lp_test_m.bin");
        return -1;
    }

    if (fread(m, sizeof(int), 1, input_m) != 1) {
        perror("Error reading m");
        fclose(input_m);
        return -1;
    }
    fclose(input_m);

    /*Reading n (columns/variables)*/
    FILE *input_n = fopen("lp_test_n.bin", "rb");
    if (input_n == NULL) {
        perror("Error reading lp_test_n.bin");
        return -1;
    }

    if (fread(n, sizeof(int), 1, input_n) != 1) {
        perror("Error reading n");
        fclose(input_n);
        return -1;
    }
    fclose(input_n);

    /*Prevents bad values*/
    if (*m <= 0 || *n <= 0) {
        fprintf(stderr, "Invalid matrix dimensions\n");
        return -1;
    }

    /*Reading c (coefficients of the objective function)*/
    FILE *input_c = fopen("lp_test_c.bin", "rb");
    if (input_c == NULL) {
        perror("Error reading lp_test_c.bin");
        return -1;
    }

    *c = malloc(*n * sizeof(double));
    if (!*c){
        fclose(input_c);
        goto cleanup;
    }

    if (fread(*c, sizeof(double), *n, input_c) != *n) {
            perror("Error reading c");
            fclose(input_c);
            goto cleanup;
    }
    fclose(input_c);

    /*Reading A (coefficients of the variables in the constraints)*/
    FILE *input_A = fopen("lp_test_A.bin", "rb");

    if (input_A == NULL) {
        perror("Error reading lp_test_A.bin");
        goto cleanup;
    }

    *A = calloc(*m, sizeof(double *));
    if (!*A){
        fclose(input_A);
        goto cleanup;
    }

    for (int i = 0; i < *m; i++) {
        (*A)[i] = malloc(*n * sizeof(double));
        if (!(*A)[i]){
            fclose(input_A);
            goto cleanup;
        }
        if (fread((*A)[i], sizeof(double), *n, input_A) != *n) {
                perror("Error reading A");
                fclose(input_A);
                goto cleanup;
            }
        }
    fclose(input_A);

    /*Reading b (right-hand side of the constraints)*/
    FILE *input_b = fopen("lp_test_b.bin", "rb");

    if (input_b == NULL) {
        perror("Error reading lp_test_b.bin");
        goto cleanup;
    }

    *b = malloc(*m * sizeof(double));
    if (!*b){
        fclose(input_b);
        goto cleanup;
    }
    if (fread(*b, sizeof(double), *m, input_b) != *m) {
            perror("Error reading b");
            fclose(input_b);
            goto cleanup;
    }
    fclose(input_b);

    /*Reading the optimal solution*/
    FILE *input_obj = fopen("lp_test_obj.bin", "rb");

    if (input_obj == NULL) {
        perror("Error reading lp_test_obj.bin");
        goto cleanup;
    }

    *optimal_solution = malloc(*n * sizeof(double));
    if (!*optimal_solution){
        fclose(input_obj);
        goto cleanup;
    }    
    if (fread(*optimal_solution, sizeof(double), *n, input_obj) != *n) {
        perror("Error reading optimal solution");
        fclose(input_obj);
        goto cleanup;
    }
    fclose(input_obj);

    return 0;

    cleanup:
        if (c && *c) { 
            free(*c); 
            *c = NULL; 
        }
        if (b && *b) { 
            free(*b); 
            *b = NULL; 
        }
        if (*A) {
            for (int i = 0; i < *m; i++) {
                if ((*A)[i]) free((*A)[i]);
            }
            free(*A);
            *A = NULL;
        }
        if (optimal_solution && *optimal_solution) {
            free(*optimal_solution);
            *optimal_solution = NULL;
        }
    return -1;
}