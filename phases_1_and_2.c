#include "phases_1_and_2.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gaussian_elimination.h"
#include "simplex.h"
#include "auxiliar_functions.h"

#define EPSILON 1e-6
#define MAX_ITERATION 100

void phases_1_and_2(double *c, double **A, double *b, int m, int n){

    /*
    FIRST PHASE
    */

    int *B_index_phase1 = malloc(m * sizeof(int));
    int *N_index_phase1 = malloc(n * sizeof(int));
    double **B_phase1 = calloc(m, sizeof(double *));
    double **N_phase1 = calloc(m, sizeof(double *));
    double *basic_cost_phase1 = malloc(m * sizeof(double));
    double *non_basic_cost_phase1 = malloc(n * sizeof(double));
    double *current_basic_x = malloc(m * sizeof(double));

    int *N_index_phase2 = NULL;
    double **N_phase2 = NULL;
    double *basic_cost_phase2 = NULL;
    double *non_basic_cost_phase2 = NULL;

    if (!B_index_phase1 || !N_index_phase1 || !B_phase1 || !N_phase1 || 
        !basic_cost_phase1 || !non_basic_cost_phase1 || !current_basic_x) {
        printf("ERROR: Not enough memory to allocate.\n");
        goto cleanup;
    }

    initialize_phase1(m, n, B_index_phase1, N_index_phase1);

    if (!initialization_phase1(B_phase1, N_phase1, A, m, n)) {
        printf("ERROR: Memory allocation failed in initialization_phase1.\n");
        goto cleanup;
    }

    for (int i = 0; i < m; i++) {
        basic_cost_phase1[i] = 1;
    }
    for (int i = 0; i < n; i++) {
        non_basic_cost_phase1[i] = 0;
    }

    int iteration = 0;
    int phase1_status = 0;
    while (phase1_status == 0 && iteration <= MAX_ITERATION) {
        phase1_status = simplex_step(B_phase1, N_phase1, b, basic_cost_phase1, non_basic_cost_phase1,
        B_index_phase1, N_index_phase1, m, m+n, &iteration);
    }

    if (verify(phase1_status, MAX_ITERATION))
        goto cleanup;

    /*Checks if the cost of optimal solution is zero*/
    double total_cost = 0;
    gaussian_elimination(B_phase1, current_basic_x, b, m); //Redefine x
    for (int i = 0; i < m; i++) {
        total_cost += basic_cost_phase1[i]*current_basic_x[i];
    }
    if (fabs(total_cost) > EPSILON) {
        printf("ERROR: First phase failed. Total cost %.6lf != 0", total_cost);
        goto cleanup;
    }
    /*
    Here, we're assuming that the artificial variables aren't basic,
    Hence, total_cost = 0 should never happen
    */

    int *B_index_phase2 = B_index_phase1;
    N_index_phase2 = malloc((n-m) * sizeof(int));
    double **B_phase2 = B_phase1;

    N_phase2 = malloc(m * sizeof(double *));
    if (!N_index_phase2 || !N_phase2) {
        goto cleanup;
    }


    for (int i = 0; i < m; i++) {
        N_phase2[i] = malloc((n-m) * sizeof(double));
        if (!N_phase2[i])
            goto cleanup;
    }

    /*
    Print x_0, ..., x_(m+n-1).
    The n variables should be 0.
    */
    for (int i = 0; i < m+n; i++) {
        if (is_in_index(B_index_phase1, m, i)) {
            int inv_index = inverse_index(B_index_phase1, m, i); //Where it is in the B_index.
            printf("x_%d: %.6lf\n", i, current_basic_x[inv_index]);
        } else
            printf("x_%d: 0\n", i);
    }
    printf("\n");


    //Filters out the artificial variables.
    int n_count = 0;
    for (int j = 0; j < n; j++) {
        if (!is_in_index(B_index_phase2, m, j)) { // If the j-th variable isn't basic, then:
            N_index_phase2[n_count] = j; // Add it to the next available slot.

            //Copy column
            for (int i = 0; i < m; i++) {
                N_phase2[i][n_count] = A[i][j]; // The next available column copies the j-th original column.
            }
            n_count++;
        }
    }

    /*
    SECOND PHASE
    */
    basic_cost_phase2 = malloc(m * sizeof(double));
    non_basic_cost_phase2 = malloc((n - m) * sizeof(double));
    if (!basic_cost_phase2 || !non_basic_cost_phase2) {
        goto cleanup;
    }

    for (int i = 0; i < m; i++) {
        int ith_basic_variable_position = B_index_phase2[i];
        basic_cost_phase2[i] = c[ith_basic_variable_position];
    }

    for (int i = 0; i < n-m; i++) {
        int ith_non_basic_variable_position = N_index_phase2[i];
        non_basic_cost_phase2[i] = c[ith_non_basic_variable_position];
    }


    /*Now, we start the second phase of the simplex method, which will find the optimal solution to the original problem.*/
    int phase2_status = 0;
    iteration = 0;
    while (phase2_status == 0 && iteration <= MAX_ITERATION) {
        phase2_status = simplex_step(B_phase2, N_phase2, b, basic_cost_phase2, non_basic_cost_phase2,
        B_index_phase2, N_index_phase2, m, n, &iteration);
    }

    if (verify(phase2_status, MAX_ITERATION))
        goto cleanup;

    //Finds the total cost of the optimal solution.
    total_cost = 0;
    gaussian_elimination(B_phase2, current_basic_x, b, m); // redefine x
    for (int i = 0; i < m; i++) {
        total_cost += basic_cost_phase2[i]*current_basic_x[i];
    }

    /*
    Prints the optimal solution of the original problem, which is the values of x_0, ..., x_(n-1).
    The remaining variables should be 0.
    */
    for (int i = 0; i < n; i++) {
        if (is_in_index(B_index_phase2, m, i)) {
            int inv_index = inverse_index(B_index_phase2, m, i); // where it is in the B_index
            printf("x_%d: %.6lf\n", i, current_basic_x[inv_index]);
        } else
            printf("x_%d: 0\n", i);
    }



cleanup:

    if (B_phase1) {
        for (int i = 0; i < m; i++) {
            if (B_phase1[i]) free(B_phase1[i]);
        }
        free(B_phase1);
    }
    if (N_phase1) {
        for (int i = 0; i < m; i++) {
            if (N_phase1[i]) free(N_phase1[i]);
        }
        free(N_phase1);
    }

    if (N_phase2) {
        for (int i = 0; i < m; i++) {
            if (N_phase2[i]) free(N_phase2[i]);
        }
        free(N_phase2);
    }
    if (N_index_phase2) free(N_index_phase2);
    if (basic_cost_phase2) free(basic_cost_phase2);
    if (non_basic_cost_phase2) free(non_basic_cost_phase2);

    if (B_index_phase1) free(B_index_phase1);
    if (N_index_phase1) free(N_index_phase1);
    if (basic_cost_phase1) free(basic_cost_phase1);
    if (non_basic_cost_phase1) free(non_basic_cost_phase1);
    if (current_basic_x) free(current_basic_x);
}