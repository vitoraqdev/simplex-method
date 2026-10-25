#include "initialize.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// LP_INPUT_FORMAT=bin selects binary; anything else (or unset) is text.
InputFormat input_format_from_env(void) {
    const char *f = getenv("LP_INPUT_FORMAT");
    return (f && strcmp(f, "bin") == 0) ? INPUT_BINARY : INPUT_TEXT;
}

// chooses path based on priority of inputs
const char *resolve_path(const char *explicit_path, InputFormat fmt, char *buf, size_t size) {
    if (explicit_path && *explicit_path) return explicit_path; // if has explicit path through argument, choose it

    const char *env_path = getenv("LP_INPUT_PATH");
    if (env_path && *env_path) return env_path; // if explicit path through environment, choose it

    const char *name = (fmt == INPUT_BINARY) ? "input.bin" : "input.txt";
    const char *dir = getenv("LP_DATA_DIR"); // if passed dir through environment choose input.txt or input.bin
    if (dir && *dir) {
        snprintf(buf, size, "%s/%s", dir, name);
        return buf;
    }
    return name; // else, return source input.bin or input.txt
}

// read m n
int read_header(FILE *f, InputFormat fmt, int *m, int *n) {
    if (fmt == INPUT_BINARY) {
        if (fread(m, sizeof(int), 1, f) != 1) return -1;
        if (fread(n, sizeof(int), 1, f) != 1) return -1;
        return 0;
    }
    return fscanf(f, "%d %d", m, n) == 2 ? 0 : -1;
}

// read matrix
int read_doubles(FILE *f, InputFormat fmt, double *dst, size_t count) {
    if (fmt == INPUT_BINARY)
        return fread(dst, sizeof(double), count, f) == count ? 0 : -1;

    for (size_t i = 0; i < count; i++)
        if (fscanf(f, "%lf", &dst[i]) != 1) return -1;
    return 0;
}


int initialize_input(double **c, double ***A, double **b, double **optimal_solution,
        int *m, int *n, InputFormat fmt, const char *path) {
    *c = NULL;
    *A = NULL;
    *b = NULL;
    *optimal_solution = NULL;

    char path_buf[1024];
    const char *file_input_path = resolve_path(path, fmt, path_buf, sizeof(path_buf));

    FILE *f = fopen(file_input_path, fmt == INPUT_BINARY ? "rb" : "r");
    if (!f) {
        fprintf(stderr, "Error opening input file: %s\n", file_input_path);
        return -1;
    }

    // read m n
    if (read_header(f, fmt, m, n) != 0) {
        fprintf(stderr, "Error reading dimensions m and n from %s\n", file_input_path);
        fclose(f);
        return -1;
    }
    if (*m <= 0 || *n <= 0 || *m > *n) {
        fprintf(stderr, "Invalid matrix dimensions m=%d, n=%d\n", *m, *n);
        fclose(f);
        return -1;
    }

    // allocate all memories needed
    *c = malloc(*n * sizeof(double));
    *A = calloc(*m, sizeof(double *));
    *b = malloc(*m * sizeof(double));
    *optimal_solution = malloc(*n * sizeof(double));
    if (!*c || !*A || !*b || !*optimal_solution) {
        fprintf(stderr, "Memory allocation failure\n");
        goto cleanup;
    }

    if (read_doubles(f, fmt, *c, *n) != 0) {
        fprintf(stderr, "Error reading cost vector c\n");
        goto cleanup;
    }
    for (int i = 0; i < *m; i++) {
        (*A)[i] = malloc(*n * sizeof(double));
        if (!(*A)[i] || read_doubles(f, fmt, (*A)[i], *n) != 0) {
            fprintf(stderr, "Error reading row A[%d]\n", i);
            goto cleanup;
        }
    }
    if (read_doubles(f, fmt, *b, *m) != 0) {
        fprintf(stderr, "Error reading vector b\n");
        goto cleanup;
    }

    fclose(f);
    return 0;

    cleanup:
    fclose(f);
    free(*c);  *c = NULL;
    free(*b);  *b = NULL;
    free(*optimal_solution); *optimal_solution = NULL;
    if (*A) {
        for (int i = 0; i < *m; i++) free((*A)[i]);   // calloc'ed so NULLs are safe
        free(*A);
        *A = NULL;
    }

    return -1;
}