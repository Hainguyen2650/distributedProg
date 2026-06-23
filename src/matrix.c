#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static double random_unit_value(unsigned int *state) {
    *state = (*state * 1103515245u + 12345u) & 0x7fffffffu;
    return (double)(*state % 1000u) / 100.0;
}

static double pattern_value(int row, int col, int variant) {
    if (variant == 0) {
        return (double)((row + col) % 7 + 1);
    }
    return (double)((row * 2 + col * 3) % 11 + 1);
}

Matrix matrix_create(int rows, int cols) {
    Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;
    matrix.data = NULL;

    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Invalid matrix shape %d x %d\n", rows, cols);
        return matrix;
    }

    matrix.data = calloc((size_t)rows * (size_t)cols, sizeof(double));
    if (!matrix.data) {
        fprintf(stderr, "Out of memory allocating matrix %d x %d\n", rows, cols);
        matrix.rows = 0;
        matrix.cols = 0;
    }

    return matrix;
}

void matrix_destroy(Matrix *matrix) {
    if (!matrix) {
        return;
    }
    free(matrix->data);
    matrix->data = NULL;
    matrix->rows = 0;
    matrix->cols = 0;
}

void matrix_fill_zero(Matrix *matrix) {
    size_t count = matrix_element_count(matrix);
    for (size_t i = 0; i < count; i++) {
        matrix->data[i] = 0.0;
    }
}

void matrix_init(Matrix *matrix, InitMode mode, int seed, int variant) {
    if (!matrix || !matrix->data) {
        return;
    }

    unsigned int state = (unsigned int)(seed + 97 * (variant + 1));
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            double value = 0.0;
            if (mode == INIT_PATTERN) {
                value = pattern_value(i, j, variant);
            } else if (mode == INIT_IDENTITY) {
                if (variant == 0) {
                    value = (i == j) ? 1.0 : 0.0;
                } else {
                    value = pattern_value(i, j, variant);
                }
            } else if (mode == INIT_RANDOM) {
                value = random_unit_value(&state);
            }
            matrix->data[i * matrix->cols + j] = value;
        }
    }
}

void matrix_multiply_add(const double *a, const double *b, double *c,
                         int rows_a, int shared, int cols_b) {
    for (int i = 0; i < rows_a; i++) {
        for (int k = 0; k < shared; k++) {
            double aik = a[i * shared + k];
            for (int j = 0; j < cols_b; j++) {
                c[i * cols_b + j] += aik * b[k * cols_b + j];
            }
        }
    }
}

void matrix_multiply_serial(const Matrix *a, const Matrix *b, Matrix *c) {
    if (!a || !b || !c || !a->data || !b->data || !c->data) {
        return;
    }
    if (a->cols != b->rows || c->rows != a->rows || c->cols != b->cols) {
        fprintf(stderr, "Invalid matrix shapes for serial multiplication\n");
        return;
    }
    matrix_fill_zero(c);
    matrix_multiply_add(a->data, b->data, c->data, a->rows, a->cols, b->cols);
}

double matrix_max_abs_error(const Matrix *actual, const Matrix *expected) {
    if (!matrix_is_same_shape(actual, expected)) {
        return INFINITY;
    }

    double max_error = 0.0;
    size_t count = matrix_element_count(actual);
    for (size_t i = 0; i < count; i++) {
        double error = fabs(actual->data[i] - expected->data[i]);
        if (error > max_error) {
            max_error = error;
        }
    }
    return max_error;
}

void matrix_print(const Matrix *matrix, const char *name) {
    if (!matrix || !matrix->data) {
        return;
    }

    printf("%s:\n", name);
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%10.2f", matrix->data[i * matrix->cols + j]);
        }
        printf("\n");
    }
}

void matrix_copy_block(double *dst, const Matrix *src, int block_rows, int block_cols,
                       int block_row, int block_col) {
    for (int i = 0; i < block_rows; i++) {
        int global_i = block_row * block_rows + i;
        for (int j = 0; j < block_cols; j++) {
            int global_j = block_col * block_cols + j;
            dst[i * block_cols + j] = src->data[global_i * src->cols + global_j];
        }
    }
}

void matrix_paste_block(Matrix *dst, const double *src, int block_rows, int block_cols,
                        int block_row, int block_col) {
    for (int i = 0; i < block_rows; i++) {
        int global_i = block_row * block_rows + i;
        for (int j = 0; j < block_cols; j++) {
            int global_j = block_col * block_cols + j;
            dst->data[global_i * dst->cols + global_j] = src[i * block_cols + j];
        }
    }
}

int matrix_is_same_shape(const Matrix *a, const Matrix *b) {
    if (!a || !b) {
        return 0;
    }
    return a->rows == b->rows && a->cols == b->cols;
}

double matrix_checksum(const Matrix *matrix) {
    if (!matrix || !matrix->data) {
        return 0.0;
    }

    double sum = 0.0;
    size_t count = matrix_element_count(matrix);
    for (size_t i = 0; i < count; i++) {
        sum += matrix->data[i];
    }
    return sum;
}

size_t matrix_element_count(const Matrix *matrix) {
    if (!matrix || !matrix->data || matrix->rows <= 0 || matrix->cols <= 0) {
        return 0;
    }
    return (size_t)matrix->rows * (size_t)matrix->cols;
}
