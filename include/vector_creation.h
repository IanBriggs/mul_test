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
void fprint_flt_vectors(FILE * stream, const flt_mat *const in);
void save_flt_vectors(char * filename, const flt_mat *const in);

void print_hex_vectors(const hex_mat *const in);
void fprint_hex_vectors(FILE * stream, const hex_mat *const in);
void save_hex_vectors(char * filename, const hex_mat *const in);

#endif
