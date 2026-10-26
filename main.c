#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "initialize.h"
#include "phases_1_and_2.h"

/*
Reads the linear optimization problem of the kind:

min c^T*x
s.a. A*x = b
x>=0

An input.txt file should be provided with the following format:
m n
c_1 c_2 ... c_n
A_11 A_12 ... A_1n
...
A_m1 A_m2 ... A_mn
b_1 b_2 ... b_m
*/

int main(int argc, char **argv) {

    double *c = NULL, *b = NULL;
    double **A = NULL;
    double *optimal_solution = NULL;
    double best_cost;
    int m, n;

    /*
     * args: if passed --binary through command then reads input.bin
     * if passed --text then reads input.txt
     * if neither of those, then reads as an explicit path for the input file
    */
    InputFormat fmt = input_format_from_env();
    const char *path = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--binary") == 0)    fmt = INPUT_BINARY;
        else if (strcmp(argv[i], "--text") == 0) fmt = INPUT_TEXT;
        else path = argv[i];
    }

    /*Input reading*/
    if (initialize_input(&c, &A, &b, &optimal_solution, &m, &n, fmt, path) != 0){
        return 1;
    }

    /*Does both phases of the simplex method*/
    phases_1_and_2(c, A, b, m, n, optimal_solution, &best_cost);

    if (A) {
        for (int i = 0; i < m; i++) {
            free(A[i]);
        }
        free(A);
    }
  
    if (c) free(c);
    if (b) free(b);
    free(optimal_solution);
    
    return 0;
}