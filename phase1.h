#ifndef SIMPLEX_METHOD_PHASE1_H
#define SIMPLEX_METHOD_PHASE1_H

void initialize_phase1(int m, int n, int *indice_B, int *indice_N);

int initialization_phase1(double **B_phase1, double **N_phase1, double **A, int rows, int cols);

void phase1(double **A, double *b, int m, int n, int max_iteration);

#endif //SIMPLEX_METHOD_PHASE1_H