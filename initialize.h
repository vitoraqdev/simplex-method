#ifndef SIMPLEX_METHOD_INITIALIZE_H
#define SIMPLEX_METHOD_INITIALIZE_H
#include <stdio.h>

typedef enum { INPUT_TEXT = 0, INPUT_BINARY = 1 } InputFormat;

int initialize_input(double **c, double ***A, double **b, double **optimal_solution,
        int *m, int *n, InputFormat fmt, const char *path);

InputFormat input_format_from_env(void);

int read_header(FILE *f, InputFormat fmt, int *m, int *n);

int read_doubles(FILE *f, InputFormat fmt, double *dst, size_t count);


#endif //SIMPLEX_METHOD_INITIALIZE_H