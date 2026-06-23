#ifndef MATRIX_H
#define MATRIX_H

#include "args.h"
#include <stddef.h>

typedef struct {
    int rows;
    int cols;
    double *data;
} Matrix;

Matrix matrix_create(int rows, int cols);
void matrix_destroy(Matrix *matrix);
void matrix_fill_zero(Matrix *matrix);
void matrix_init(Matrix *matrix, InitMode mode, int seed, int variant);
void matrix_multiply_add(const double *a, const double *b, double *c,
                         int rows_a, int shared, int cols_b);
void matrix_multiply_serial(const Matrix *a, const Matrix *b, Matrix *c);
double matrix_max_abs_error(const Matrix *actual, const Matrix *expected);
void matrix_print(const Matrix *matrix, const char *name);
void matrix_copy_block(double *dst, const Matrix *src, int block_rows, int block_cols,
                       int block_row, int block_col);
void matrix_paste_block(Matrix *dst, const double *src, int block_rows, int block_cols,
                        int block_row, int block_col);
int matrix_is_same_shape(const Matrix *a, const Matrix *b);
double matrix_checksum(const Matrix *matrix);
size_t matrix_element_count(const Matrix *matrix);

#endif
