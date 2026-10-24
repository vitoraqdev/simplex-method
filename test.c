#include "test.h"

#include "gaussian_elimination.h"
#include "phases_1_and_2.h"
#include "simplex.h"

// TODO: fazer testes unitarios das funções mais básicas, sem usar necessariamente o fixture
// --- TESTES UNITÁRIOS ---

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
    double *normal_cost;
    double *lambda;
    double *current_basic_x;
    double *reduced_cost;
    double *direction;
    int *basic_index;
    int *normal_index;
    double step_size;
    double tolerance;
};

// Setup tests
// Our Oracle, all known results are here already, on each test function we create a new variable
// and test against our Oracle.
UTEST_F_SETUP(SimplexFixture) {
    utest_fixture->m = 2;
    utest_fixture->n = 5;
    utest_fixture->step_size = 15.0/7.0;
    utest_fixture->tolerance = 1e-6;

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
    utest_fixture->basic_cost = malloc(utest_fixture->m * sizeof(double));
    utest_fixture->normal_cost = malloc(utest_fixture->N_col_size * sizeof(double));
    utest_fixture->lambda = malloc(utest_fixture->N_col_size * sizeof(double));
    utest_fixture->current_basic_x = malloc(utest_fixture->m * sizeof(double));
    utest_fixture->reduced_cost = malloc(utest_fixture->N_col_size * sizeof(double));
    utest_fixture->direction = malloc(utest_fixture->m * sizeof(double));
    utest_fixture->basic_index = malloc(utest_fixture->m * sizeof(int));
    utest_fixture->normal_index = malloc(utest_fixture->N_col_size * sizeof(int));


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

    // c_n = [-2 -3 0]^T
    utest_fixture->normal_cost[0] = -2.0;
    utest_fixture->normal_cost[1] = -3.0;
    utest_fixture->normal_cost[2] = 0.0;

    // x_B = [5; 5]
    utest_fixture->current_basic_x[0] = 5;
    utest_fixture->current_basic_x[1] = 5;

    // lambda = [0 -4/3]^T
    utest_fixture->lambda[0] = 0;
    utest_fixture->lambda[1] = -4.0/3.0;

    // reduced cost = [2/3 11/3 4/3]^T
    utest_fixture->reduced_cost[0] = 2.0/3.0;
    utest_fixture->reduced_cost[1] = 11.0/3.0;
    utest_fixture->reduced_cost[2] = 4.0/3.0;

    // direction = [2/3 7/3]^T
    utest_fixture->direction[0] = 2.0/3.0;
    utest_fixture->direction[1] = 7.0/3.0;

    // basic_index = [3 4]^T
    utest_fixture->basic_index[0] = 3;
    utest_fixture->basic_index[1] = 4;

    // normal_index = [1 2 4]^T
    utest_fixture->normal_index[0] = 1;
    utest_fixture->normal_index[1] = 2;
    utest_fixture->normal_index[2] = 5;

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
    free(utest_fixture->normal_cost);
    free(utest_fixture->lambda);
    free(utest_fixture->current_basic_x);
    free(utest_fixture->reduced_cost);
    free(utest_fixture->direction);
    free(utest_fixture->basic_index);
    free(utest_fixture->normal_index);
}

UTEST_F(SimplexFixture, test_gaussian_elimination) {
    double *result_current_basic_x = malloc(utest_fixture->m*sizeof(double));
    gaussian_elimination(utest_fixture->B, result_current_basic_x, utest_fixture->b, utest_fixture->m);
    EXPECT_NEAR(utest_fixture->current_basic_x[0], result_current_basic_x[0], utest_fixture->tolerance);
    EXPECT_NEAR(utest_fixture->current_basic_x[1], result_current_basic_x[1], utest_fixture->tolerance);
    free(result_current_basic_x);
}

UTEST_F(SimplexFixture, test_simplex_multiplier_vector) {
    double *result_lambda = malloc(utest_fixture->m*sizeof(double));
    multiplier_simplex_vector(utest_fixture->B, result_lambda, utest_fixture->basic_cost, utest_fixture->m);
    EXPECT_NEAR(utest_fixture->lambda[0], result_lambda[0], utest_fixture->tolerance);
    EXPECT_NEAR(utest_fixture->lambda[1], result_lambda[1], utest_fixture->tolerance);
    free(result_lambda);
}

UTEST_F(SimplexFixture, test_reduced_cost) {
    double *result_reduced_cost = malloc(utest_fixture->N_col_size*sizeof(double));
    find_reduced_cost(utest_fixture->normal_cost, utest_fixture->lambda, utest_fixture->N, utest_fixture->m,
        utest_fixture->n, result_reduced_cost);
    EXPECT_NEAR(utest_fixture->reduced_cost[0], result_reduced_cost[0], utest_fixture->tolerance);
    EXPECT_NEAR(utest_fixture->reduced_cost[1], result_reduced_cost[1], utest_fixture->tolerance);
    EXPECT_NEAR(utest_fixture->reduced_cost[2], result_reduced_cost[2], utest_fixture->tolerance);
    free(result_reduced_cost);
}

UTEST_F(SimplexFixture, test_normal_variable_to_enter) {
    int normal_variable_index = normal_variable_index_to_enter(utest_fixture->reduced_cost, utest_fixture->N_col_size);
    ASSERT_EQ(normal_variable_index, -1); // HARD CODED
}

UTEST_F(SimplexFixture, test_basic_variable_index_to_exit) {
    int basic_variable_index = basic_variable_index_to_exit(utest_fixture->current_basic_x, utest_fixture->direction,
        utest_fixture->m);
    ASSERT_EQ(basic_variable_index, 1); // HARD CODED
}

UTEST_F(SimplexFixture, test_simplex_step) {
    int iteration = 0;
    int STATUS = simplex_step(utest_fixture->B, utest_fixture->N, utest_fixture->b, utest_fixture->basic_cost,
        utest_fixture->normal_cost, utest_fixture->basic_index, utest_fixture->normal_index, utest_fixture->m,
        utest_fixture->n, &iteration);
    ASSERT_EQ(1, STATUS);
}


static void load_binary_matrix(const char *filename, int rows, int cols, double **A) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Could not open binary matrix file %s\n", filename);
        exit(1);
    }
    for (int i = 0; i < rows; i++) {
        fread(A[i], sizeof(double), cols, f);
    }
    fclose(f);
}

static void load_binary_vector(const char *filename, int size, double *v) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Could not open binary vector file %s\n", filename);
        exit(1);
    }
    fread(v, sizeof(double), size, f);
    fclose(f);
}

/* THIS WAS FOR DEBUGGING PURPOSES, CAN KEEP THIS LATER FOR AN EVEN MORE INTEGRATED TEST
 * USING PYTHON WE CAN GET ALL VARIABLES, EXPORT TO ONE SINGLE FILE, AND USE IT IN THE TESTS
*/
// UTEST(gaussian_elimination, random_100x100) {
//     int n = 100;
//     double **A = malloc(n * sizeof(double *));
//     for (int i = 0; i < n; i++) {
//         A[i] = malloc(n * sizeof(double));
//     }
//     double *b = malloc(n * sizeof(double));
//     double *expected_x = malloc(n * sizeof(double));
//     double *result_x = malloc(n * sizeof(double));
//
//     // Load matrices/vectors via your binary helpers
//     load_binary_matrix("../ge_test_A.bin", n, n, A);
//     load_binary_vector("../ge_test_b.bin", n, b);
//     load_binary_vector("../ge_test_x.bin", n, expected_x);
//
//     // Run the solver function
//     gaussian_elimination(A, result_x, b, n);
//
//     // Assert correctness against NumPy's expected solution
//     double tol = 1e-4;
//     for (int i = 0; i < n; i++) {
//         EXPECT_NEAR(expected_x[i], result_x[i], tol);
//     }
//
//     // Cleanup
//     for (int i = 0; i < n; i++) {
//         free(A[i]);
//     }
//     free(A);
//     free(b);
//     free(expected_x);
//     free(result_x);
// }
//
// UTEST(simplex_math, simplex_multiplier_vector_random) {
//     int m = 100;
//
//     // Allocate memory
//     double **B = malloc(m * sizeof(double *));
//     for (int i = 0; i < m; i++) {
//         B[i] = malloc(m * sizeof(double));
//     }
//     double *c_B = malloc(m * sizeof(double));
//     double *expected_lambda = malloc(m * sizeof(double));
//     double *result_lambda = malloc(m * sizeof(double));
//
//     // Load matrices/vectors generated from Python (adjust path if running from build/)
//     load_binary_matrix("../lambda_B.bin", m, m, B);
//     load_binary_vector("../lambda_c_B.bin", m, c_B);
//     load_binary_vector("../lambda.bin", m, expected_lambda);
//
//     // Call your function under test
//     // Note: multiplier_simplex_vector takes (B, result_lambda, basic_cost, m)
//     multiplier_simplex_vector(B, result_lambda, c_B, m);
//
//     // Assert correctness against NumPy's direct solve solution
//     double tol = 1e-4;
//     for (int i = 0; i < m; i++) {
//         EXPECT_NEAR(expected_lambda[i], result_lambda[i], tol);
//     }
//
//     // Cleanup
//     for (int i = 0; i < m; i++) {
//         free(B[i]);
//     }
//     free(B);
//     free(c_B);
//     free(expected_lambda);
//     free(result_lambda);
// }

static void load_binary_int(const char *filename, int *val) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Could not open binary file %s\n", filename);
        exit(1);
    }
    fread(val, sizeof(int), 1, f);
    fclose(f);
}

// Massive integration test
UTEST(integration, large_scale) {
    int rows, cols;

    load_binary_int("../lp_test_m.bin", &rows);
    load_binary_int("../lp_test_n.bin", &cols);

    double *result_solution = malloc(cols * sizeof(double));
    double result_cost = 0.0;
    double expected_cost = 0.0;

    double *b = malloc(rows * sizeof(double));
    double *c = malloc(cols * sizeof(double));
    double **A = malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++) {
        A[i] = malloc(cols * sizeof(double));
    }

    // Load matrices instantly via pure raw binary
    load_binary_matrix("../lp_test_A.bin", rows, cols, A);
    load_binary_vector("../lp_test_b.bin", rows, b);
    load_binary_vector("../lp_test_c.bin", cols, c);
    load_binary_vector("../lp_test_obj.bin", 1, &expected_cost);

    int result = phases_1_and_2(c, A, b, rows, cols, result_solution, &result_cost);

    // Assert solver success and correctness
    double tol = 1e-3;
    EXPECT_EQ(1, result);
    EXPECT_NEAR(expected_cost, result_cost, tol);
    for (int j = 0; j < cols; j++) {
        EXPECT_GE(result_solution[j], -tol); // x_optimal >= 0
    }
    for (int i = 0; i < rows; i++) {
        double row_sum = 0.0;
        for (int j = 0; j < cols; j++) {
            row_sum += A[i][j] * result_solution[j];
        }
        EXPECT_NEAR(row_sum, b[i], tol);
    }

    free(result_solution);
    free(b);
    free(c);
    for (int i = 0; i < rows; i++) {
        free(A[i]);
    }
    free(A);
}



UTEST_MAIN();