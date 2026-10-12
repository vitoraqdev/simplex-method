#include "test.h"

#include "gaussian_elimination.h"

// todo: add fixture testcases, initializing a struct and using it across all simplex functions?

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

UTEST(matrix, test_gaussian_elimination) {
    int n = 3;

    // Allocate matrix A (3x3)
    double **A = malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        A[i] = (double *)malloc(n * sizeof(double));
    }

    // Allocate vectors b and x
    double *b = malloc(n * sizeof(double));
    double *x = malloc(n * sizeof(double));

    // Define a test system:
    // 2x + 1y + 1z = 4   -> x = [1, 1, 1]
    // 4x + 3y + 3z = 10
    // 8x + 7y + 9z = 24
    double initial_A[3][3] = {
        {2.0f, 1.0f, 1.0f},
        {4.0f, 3.0f, 3.0f},
        {8.0f, 7.0f, 9.0f}
    };
    double initial_b[3] = {4.0f, 10.0f, 24.0f};

    for (int i = 0; i < n; i++) {
        b[i] = initial_b[i];
        x[i] = 0.0f;
        for (int j = 0; j < n; j++) {
            A[i][j] = initial_A[i][j];
        }
    }

    // Run solver
    int status = gaussian_elimination(A, x, b, n);
    if (status != 0) {
        // Cleanup memory
        for (int i = 0; i < n; i++) {
            free(A[i]);
        }
        free(A);
        free(b);
        free(x);
        ASSERT_EQ(status, 0);
    }

    // Verify expected solution: x should be roughly [1.0, 1.0, 1.0]
    double tolerance = 1e-5f;
    printf("Resulting x: [%f, %f, %f]\n", x[0], x[1], x[2]);

    for (int i = 0; i < n; i++) {
        ASSERT_NEAR(x[i], 1.0, tolerance);
    }

    // Cleanup memory
    for (int i = 0; i < n; i++) {
        free(A[i]);
    }
    free(A);
    free(b);
    free(x);
}

UTEST_MAIN();
