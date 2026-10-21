#ifndef SIMPLEX_METHOD_AUXILIAR_FUNCTIONS_H
#define SIMPLEX_METHOD_AUXILIAR_FUNCTIONS_H

int is_in_index(int *index, int length, int variable);

int inverse_index(int *index, int length, int variable);

int verify(int phase1_status, int max_iteration);

#endif //SIMPLEX_METHOD_AUXILIAR_FUNCTIONS_H