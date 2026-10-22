#ifndef SIMPLEX_METHOD_AUXILIAR_FUNCTIONS_H
#define SIMPLEX_METHOD_AUXILIAR_FUNCTIONS_H

int is_in_index(int *index, int length, int variable);

int inverse_index(int *index, int length, int variable);

int verify(int phase1_status, int max_iteration);

void initialize_phase1(int m, int n, int *indice_B, int *indice_N);

int initialization_phase1(double **B_phase1, double **N_phase1, double **A, int rows, int cols);

#endif //SIMPLEX_METHOD_AUXILIAR_FUNCTIONS_H