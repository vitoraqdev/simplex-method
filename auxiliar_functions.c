#include "auxiliar_functions.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int is_in_index(int *index, int length, int variable) {
    int is_inside = 0;
    for (int i = 0; i < length; i++) {
        if (index[i] == variable)
            is_inside = 1;
    }

    return is_inside;
}

int inverse_index(int *index, int length, int variable) {
    if (!is_in_index(index, length, variable))
        return -1;

    for (int i = 0; i < length; i++) {
        if (index[i] == variable)
            return i;
    }
}

int verify(int phase1_status, int max_iteration){
    if (phase1_status == -2) {
        printf("ERROR: No finite solution for the first phase.");
        return 1;
    }
    if (phase1_status == -1) {
        printf("ERROR: Not enough memory to allocate.");
        return 1;
    }
    if (phase1_status == 0) {
        printf("ERROR: Max iteration (%d) reached.", max_iteration);
        return 1;
    }
    return 0;
}