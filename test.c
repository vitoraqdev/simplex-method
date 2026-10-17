#include "test.h"

#include "gaussian_elimination.h"
#include "simplex.h"


// --- TESTES UNITÁRIOS ---
// UTEST(matrix, test_max_from_column) {
//     double row0[5] = {3, 2, 1, 1, 0};
//     double row1[5] = {2, 5, 3, 0, 1};
//
//     double *A[2] = {row0, row1};
//
//     ASSERT_EQ(0, max_from_column(A, 2, 5, 0));
//     ASSERT_EQ(1, max_from_column(A, 2, 5, 1));
//     ASSERT_EQ(1, max_from_column(A, 2, 5, 2));
//     ASSERT_EQ(0, max_from_column(A, 2, 5, 3));
//     ASSERT_EQ(1, max_from_column(A, 2, 5, 4));
// }

// UTEST (matrix, test_row_exchange) {
//     int rows = 2;
//     int cols = 5;
//
//     double row0[5] = {3, 2, 1, 1, 0};
//     double row1[5] = {2, 5, 3, 0, 1};
//
//     double **A = malloc(rows*sizeof(double *));
//     A[0] = malloc(cols*sizeof(double));
//     A[1] = malloc(cols*sizeof(double));
//     memcpy(A[0], row0, sizeof(row0));
//     memcpy(A[1], row1, sizeof(row1));
//
//     double **B = malloc(rows*sizeof(double *));
//     B[0] = malloc(cols*sizeof(double));
//     B[1] = malloc(cols*sizeof(double));
//     memcpy(B[0], A[0], sizeof(row0));
//     memcpy(B[1], A[1], sizeof(row1));
//
//     swap_rows(A, 2, 5, 0, 1);
//
//     for (int i = 0; i < 2; i++) {
//         for (int j = 0; j < 5; j++) {
//             ASSERT_EQ(B[i][j], A[1-i][j]);
//         }
//         free(A[1-i]);
//         free(B[i]);
//     }
//
//     free(A);
//     free(B);
//
// }

// Fixture is a structure that will be used and initialized for every test that calls it
// the initialization is in SETUP, it will be an example from wikipedia
struct SimplexFixture {
    int m;
    int n;
    int N_col_size;
    double **A;
    double **B;
    double **N;
    double *b;
    double *c;
    double *basic_cost;
    double *lambda;
    double *x;
    double *reduced_cost;
    double *direction;
    double step_size;
    double tolerance;
};

// Setup tests
UTEST_F_SETUP(SimplexFixture) {
    utest_fixture->m = 2;
    utest_fixture->n = 5;

    // Allocate matrix A (2x5) and B (2x2)
    utest_fixture->A = malloc(utest_fixture->m * sizeof(double *));
    utest_fixture->B = malloc(utest_fixture->m * sizeof(double *));
    for (int i = 0; i < utest_fixture->m; i++) {
        utest_fixture->A[i] = malloc(utest_fixture->n * sizeof(double));
        utest_fixture->B[i] = malloc(utest_fixture->m * sizeof(double));
    }

    // Allocate matrix N (2x3)
    utest_fixture->N_col_size = utest_fixture->n - utest_fixture->m;
    utest_fixture->N = malloc(utest_fixture->m * sizeof(double *));
    for (int i = 0; i < utest_fixture->m; i++) {
        utest_fixture->N[i] = malloc(utest_fixture->N_col_size * sizeof(double));
    }

    // Allocate vectors
    utest_fixture->b = malloc(utest_fixture->m * sizeof(double));
    utest_fixture->c = malloc(utest_fixture->n * sizeof(double));
    utest_fixture->basic_cost = malloc(utest_fixture->N_col_size * sizeof(double));
    utest_fixture->lambda = malloc(utest_fixture->N_col_size * sizeof(double));
    utest_fixture->x = malloc(utest_fixture->n * sizeof(double));
    utest_fixture->reduced_cost = malloc(utest_fixture->N_col_size * sizeof(double));
    utest_fixture->direction = malloc(utest_fixture->m * sizeof(double));


    // Using wikipedia https://en.wikipedia.org/wiki/Revised_simplex_method numerical example
    // A = [3 2 1 1 0;
    //      2 5 3 0 1]

    double initial_A[2][5] = {
        {3.0f, 2.0f, 1.0f, 1.0f, 0.0f},
        {2.0f, 5.0f, 3.0f, 0.0f, 1.0f}
    };

    for (int i = 0; i < utest_fixture->m; i++) {
        for (int j = 0; j < utest_fixture->n; j++) {
            (utest_fixture->A)[i][j] = initial_A[i][j];
        }
    }

    // b = [10 15]^T
    utest_fixture->b[0] = 10;
    utest_fixture->b[1] = 15;

    // c = [-2 -3 -4 0 0]^T
    utest_fixture->c[0] = -2;
    utest_fixture->c[1] = -3;
    utest_fixture->c[2] = -4;
    utest_fixture->c[3] = 0;
    utest_fixture->c[4] = 0;

    // B = [1 1; 3 0]
    for (int i = 0; i < utest_fixture->m; i++) {
        for (int j = 0; j < utest_fixture->m; j++) {
            // copy the 3th and 4th column from A to B
            utest_fixture->B[i][j] = utest_fixture->A[i][j+2];
        }
    }

    // N = [3 2 0; 2 5 1]
    for (int i = 0; i < utest_fixture->m; i++) {
        // copy the 1st, 2nd and 5th column from A to B
        utest_fixture->N[i][0] = utest_fixture->A[i][0];
        utest_fixture->N[i][1] = utest_fixture->A[i][1];
        utest_fixture->N[i][2] = utest_fixture->A[i][4];
    }

    // c_b = [-4 0]^T
    utest_fixture->basic_cost[0] = -4.0;
    utest_fixture->basic_cost[1] = 0.0;
}



// Cleanup memory, it calls everytime it exits a test function, we clear all variables to not leak memory
UTEST_F_TEARDOWN(SimplexFixture) {
    for (int i = 0; i < utest_fixture->m; i++) {
        free(utest_fixture->A[i]);
        free(utest_fixture->B[i]);
        free(utest_fixture->N[i]);
    }

    free(utest_fixture->A);
    free(utest_fixture->B);
    free(utest_fixture->N);
    free(utest_fixture->b);
    free(utest_fixture->c);
    free(utest_fixture->basic_cost);
    free(utest_fixture->lambda);
    free(utest_fixture->x);
    free(utest_fixture->reduced_cost);
    free(utest_fixture->direction);
}

UTEST_F(SimplexFixture, test_gaussian_elimination) {
    gaussian_elimination(utest_fixture->B, utest_fixture->x, utest_fixture->b, utest_fixture->m);
    ASSERT_NEAR(utest_fixture->x[0], 5, utest_fixture->tolerance);
    ASSERT_NEAR(utest_fixture->x[1], 5, utest_fixture->tolerance);
}

UTEST_F(SimplexFixture, test_simplex_multiplier_vector) {
    multiplier_simplex_vector(utest_fixture->B, utest_fixture->lambda, utest_fixture->basic_cost, utest_fixture->m);
    ASSERT_NEAR(utest_fixture->lambda[0], 0.0, utest_fixture->tolerance);
    ASSERT_NEAR(utest_fixture->lambda[1], -4.0/3.0, utest_fixture->tolerance);
}

UTEST_MAIN();
