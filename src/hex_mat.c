#include <assert.h>
#include <stdio.h>

#include "hex_mat.h"

void
hex_mat_ok(const hex_mat *const in)
{
  assert(in != NULL);
  assert(in->matrix != NULL);

  for (size_t x_index=0; x_index<in->width; x_index++) {
    assert(in->matrix[x_index] != NULL);
  }
}


hex_mat*
malloc_hex_mat(const size_t width, const size_t height)
{
  hex_mat *const retval = malloc(sizeof(hex_mat));
  assert(retval != NULL);

  retval->matrix =malloc(width * sizeof(hex*));
  assert(retval->matrix != NULL);

  for (size_t x_index=0; x_index<width; x_index++) {
    retval->matrix[x_index] = malloc(height * sizeof(hex));
    assert(retval->matrix[x_index] != NULL);
  }

  retval->width = width;
  retval->height = height;

  hex_mat_ok(retval);
  return retval;
}


void
free_hex_mat(hex_mat *in)
{
  hex_mat_ok(in);

  for (size_t x_index=0; x_index<in->width; x_index++) {
    free(in->matrix[x_index]);
    in->matrix[x_index] = NULL;
  }

  free(in->matrix);
  in->matrix = NULL;
  
  free(in);
}


hex_mat* 
copy_hex_mat(const hex_mat *const in)
{
  hex_mat_ok(in);

  hex_mat *retval = malloc_hex_mat(in->width, in->height);
  
  for (size_t y_index=0; y_index<in->width; y_index++) {  
    for (size_t x_index=0; x_index<in->width; x_index++) {
      retval->matrix[x_index][y_index] = in->matrix[x_index][y_index];
    }
  }
  
  hex_mat_ok(retval);
  return retval;
}


void
print_hex_mat(const hex_mat *const in)
{
  hex_mat_ok(in);

  printf("%lu\n%lu\n", in->width, in->height);

  for (size_t y_index=0; y_index<in->height; y_index++) {  
    size_t x_index;
    for (x_index=0; x_index<in->width-1; x_index++) {
      printf("%u ", in->matrix[x_index][y_index]);
    }
    printf("%u\n", in->matrix[x_index][y_index]);
  }
}
