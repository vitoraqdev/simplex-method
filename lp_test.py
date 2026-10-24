import numpy as np

"""
Generates a feasible and bounded LP problem

min c^T*x
Ax = b
x >= 0

Outputs 6 binary files, two for m and n, one for each matrix and one for the optimal solution, to plug into the simplex testing.

We are going to generate A_{mxn} and x_optimal randomly, and force the first m columns of A to be the identity,
then solve b = A*x, and since we want x_optimal to be the solution, reduced cost must be >= 0 (minimization problem).
=>  ĉ_Nj = c_Nj - λ^T * a_Nj >= 0
=>  c_Nj >= λ^T * a_Nj


#include <stdio.h>

#include <stdlib.h>


int initialize_input(double **c, double ***A, double **b, double **optimal_solution, unsigned int *m, unsigned int *n){

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


    if (fread(m, sizeof(unsigned int), 1, input_m) != 1) {

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


    if (fread(n, sizeof(unsigned int), 1, input_n) != 1) {

        perror("Error reading n");

        fclose(input_n);

        return -1;

    }

    fclose(input_n);


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
=>  c_Nj = λ^T * a_Nj + slack,  slack >= 0
Since B = I_m
=>  c_Nj = c_B^T * B^-1 * a_Nj + slack
=>  c_Nj = c_B^T * a_Nj + slack
=>  c_N = c_B^T * N + slack

So we can just generate c_B and a slack for each j, and calculate c_N so that x_optimal is optimal.
Therefore we have a feasible bounded LP.

Optimal cost is
    c_B * x_B = c_B * b 

For testing purposes I'm going to permutate matrix A to remove the identity from the starting columns, as well as all
other variables. 
"""

import os
m = int(os.environ.get("LP_TEST_M", 100)) # default 100
n = int(os.environ.get("LP_TEST_N", 200)) # default 200

RNG = np.random.default_rng(42)

A = RNG.random((m, n))

A[:m, :m] = np.eye(m)

x_optimal = RNG.random(n)
b = A @ x_optimal
c_B = RNG.random(m)
slack = RNG.random(n-m)

c_N = c_B @ A[:, m:] + slack

c = np.concatenate((c_B, c_N))

optimal_cost = c_B @ b


"""
Permutate resetting the seed, to have same permutation for all matrices
"""

RNG = np.random.default_rng(42)
RNG.shuffle(A, axis=1)
RNG = np.random.default_rng(42)
RNG.shuffle(c)

# print(A[:m, :m])
# print(c[:m])
# print(optimal_cost)
# Output to bin file

np.array([m], dtype=np.int32).tofile("lp_test_m.bin")
np.array([n], dtype=np.int32).tofile("lp_test_n.bin")

np.ascontiguousarray(A).astype(np.float64).tofile("lp_test_A.bin")
b.astype(np.float64).tofile("lp_test_b.bin")
c.astype(np.float64).tofile("lp_test_c.bin")

np.array([optimal_cost], dtype=np.float64).tofile("lp_test_obj.bin")