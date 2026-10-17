#ifndef SIMPLEX_METHOD_SIMPLEX_H
#define SIMPLEX_METHOD_SIMPLEX_H

// Avisa o C que a função existe e quais parâmetros recebe
int multiplier_simplex_vector(double **base, double *lambda, double *base_cost, int tam);

int reduced_cost(double *normal_cost, double *lambda, double **N, int m, int n, double *reduced_cost);

#endif //SIMPLEX_METHOD_SIMPLEX_H
