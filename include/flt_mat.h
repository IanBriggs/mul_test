#ifndef FLT_MAT_H
#define FLT_MAT_H

#include <stdlib.h>

#include "float_utils.h"

typedef struct _flt_mat {
  size_t width;
  size_t height;
  flt **matrix;
} flt_mat;

void flt_mat_ok(const flt_mat *const in);
flt_mat* malloc_flt_mat(const size_t width, const size_t height);
flt_mat* copy_flt_mat(const flt_mat *const in);
void free_flt_mat(flt_mat *in);
void print_flt_mat(const flt_mat *const in);



#endif
