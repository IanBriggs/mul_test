#include <assert.h>
#include <stdio.h>

#include "flt_mat.h"


void
flt_mat_ok(const flt_mat *const in)
{
  assert(in != NULL);
  assert(in->matrix != NULL);

  for (size_t x_index=0; x_index<in->width; x_index++) {
    assert(in->matrix[x_index] != NULL);
  }
}


flt_mat*
malloc_flt_mat(const size_t width, const size_t height)
{
  flt_mat *const retval = malloc(sizeof(flt_mat));
  assert(retval != NULL);

  retval->matrix =malloc(width * sizeof(flt*));
  assert(retval->matrix != NULL);

  for (size_t x_index=0; x_index<width; x_index++) {
    retval->matrix[x_index] = malloc(height * sizeof(flt));
    assert(retval->matrix[x_index] != NULL);
  }

  retval->width = width;
  retval->height = height;

  flt_mat_ok(retval);
  return retval;
}


void
free_flt_mat(flt_mat *in)
{
  flt_mat_ok(in);

  for (size_t x_index=0; x_index<in->width; x_index++) {
    free(in->matrix[x_index]);
    in->matrix[x_index] = NULL;
  }

  free(in->matrix);
  in->matrix = NULL;
  
  free(in);
}


flt_mat* 
copy_flt_mat(const flt_mat *const in)
{
  flt_mat_ok(in);

  flt_mat *retval = malloc_flt_mat(in->width, in->height);
  
  for (size_t y_index=0; y_index<in->width; y_index++) {  
    for (size_t x_index=0; x_index<in->width; x_index++) {
      retval->matrix[x_index][y_index] = in->matrix[x_index][y_index];
    }
  }
  
  flt_mat_ok(retval);
  return retval;
}


void
print_flt_mat(const flt_mat *const in)
{
  fprint_flt_mat(stdout, in);
}


void
save_flt_mat(char * filename, const flt_mat *const in)
{
  FILE * fp = fopen(filename, "w");
  assert(fp != NULL);
  fprint_flt_mat(fp, in);
}


void
fprint_flt_mat(FILE* stream, const flt_mat *const in)
{
  flt_mat_ok(in);

  //fprintf(stream, "%lu\n%lu\n", in->width, in->height);

  for (size_t y_index=0; y_index<in->height; y_index++) {  
    size_t x_index;
    for (x_index=0; x_index<in->width-1; x_index++) {
      fprintf(stream, "%f ", in->matrix[x_index][y_index]);
    }
    fprintf(stream, "%f\n", in->matrix[x_index][y_index]);
  }
}
