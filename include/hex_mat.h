#ifndef HEX_MAT_H
#define HEX_MAT_H

#include <stdlib.h>

#include "float_utils.h"

typedef struct _hex_mat {
  size_t width;
  size_t height;
  hex **matrix;
} hex_mat;

void hex_mat_ok(const hex_mat *const in);
hex_mat* malloc_hex_mat(const size_t width, const size_t height);
hex_mat* copy_hex_mat(const hex_mat *const in);
void free_hex_mat(hex_mat *in);
void print_hex_mat(const hex_mat *const in);



#endif
