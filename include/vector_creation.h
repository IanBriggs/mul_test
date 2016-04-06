#ifndef VECTOR_CREATION_H
#define VECTOR_CREATION_H

#include "float_utils.h"
#include "flt_mat.h"
#include "hex_mat.h"


typedef struct _vectors {
  flt_mat* average;
  hex_mat* mulhi;
  hex_mat* mullo;
} vectors;

vectors* create_vectors(const flt_mat const* in, const hex m, const size_t H, const size_t L);

void print_flt_vectors(const flt_mat *const in);

void print_hex_vectors(const hex_mat *const in);

#endif
