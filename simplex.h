#ifndef SIMPLEX_METHOD_SIMPLEX_H
#define SIMPLEX_METHOD_SIMPLEX_H
#include <stddef.h>

typedef enum {
    FIRST_PHASE_COST_NOT_ZERO = -4,
    MAX_ITERATION_REACHED = -3,
    SIMPLEX_NO_FINITE_SOLUTION = -2,
    SIMPLEX_MEMORY_FAILURE = -1,
    SIMPLEX_KEEP_GOING = 0,
    SIMPLEX_OPTIMAL = 1
} SimplexStatus;


int multiplier_simplex_vector(double **base, double *lambda, double *base_cost, int tam);

int find_reduced_cost(double *normal_cost, double *lambda, double **N, int m, int n, double *reduced_cost);

int get_column(double **matrix, int rows, int column_index, double *column_vector);

int find_direction(double **B, double *direction, int tam, double *A_column);

int normal_variable_index_to_enter(double *reduced_cost, int N_cols_size);

int basic_variable_index_to_exit(double *current_basic_x, double *direction, int tam);

double find_step_size(double *current_basic_x, double *direction, int index);

void generic_swap(void *a, void *b, size_t size);

int switch_basic_and_normal_column(double **B, double **N, int base_col_index, int normal_col_index, int rows);

int simplex_step(double **B, double **N, double *b, double *basic_cost, double *normal_cost, int *basic_index,
    int *normal_index, int rows, int cols, int *iteration);

int reduced_cost(double *normal_cost, double *lambda, double **N, int m, int n, double *reduced_cost);

#endif //SIMPLEX_METHOD_SIMPLEX_H
