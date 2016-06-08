#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "vector_creation.h"


hex
hex_sum(hex* in, size_t len)
{
  hex sum = 0;
  for (size_t i=0; i<len; i++) {
    sum += in[i];
  }

  return sum;
}


hex
average_hex(hex* in, size_t len)
{

  hex retval = hex_sum(in, len)/len;

  return retval;
}


flt
average_flt(flt* in, size_t len)
{

  flt retval = flt_sum(in, len)/len;

  if (isinf(retval)) {
    for (size_t i=0; i<len; i++) {
      in[i] /= len;
    }
    retval = flt_sum(in, len);
  }

  return retval;
}


hex*
linearize_hex_mat(const hex_mat *const in, const size_t x, const size_t y, const size_t L)
{
  
  hex* retval = malloc(L*L*sizeof(hex));
  assert(retval != NULL);

  size_t which_entry = 0;
  for (size_t y_index=y*L; y_index<(y+1)*L; y_index++) {
    for (size_t x_index=x*L; x_index<(x+1)*L; x_index++) {
      retval[which_entry] = in->matrix[x_index][y_index];
      which_entry++;
    }
  }

  return retval;
}


flt*
linearize_flt_mat(const flt_mat *const in, size_t x, size_t y, size_t L)
{
  
  flt* retval = malloc(L*L*sizeof(flt));
  assert(retval != NULL);

  size_t which_entry = 0;
  for (size_t y_index=y*L; y_index<(y+1)*L; y_index++) {
    for (size_t x_index=x*L; x_index<(x+1)*L; x_index++) {
      retval[which_entry] = in->matrix[x_index][y_index];
      which_entry++;
    }
  }

  return retval;
}


hex_mat*
scale_hex_mat(const hex_mat *const in, const size_t L)
{
  
  const size_t new_width = in->width/L;
  const size_t new_height = in->height/L;

  hex_mat *const retval = malloc_hex_mat(new_width, new_height);

  for (size_t x_index=0; x_index<new_width; x_index++) {
    for (size_t y_index=0; y_index<new_height; y_index++) {
      hex* values = linearize_hex_mat(in, x_index, y_index, L);
      size_t len = L*L;
      retval->matrix[x_index][y_index] = average_hex(values, len);
    }
  }

  return retval;
}


flt_mat*
scale_flt_mat(const flt_mat *const in, const size_t L)
{
  
  const size_t new_width = in->width/L;
  const size_t new_height = in->height/L;

  flt_mat *const retval = malloc_flt_mat(new_width, new_height);
  assert(retval != NULL);

  for (size_t x_index=0; x_index<new_width; x_index++) {
    for (size_t y_index=0; y_index<new_height; y_index++) {
      flt* values = linearize_flt_mat(in, x_index, y_index, L);
      size_t len = L*L;
      retval->matrix[x_index][y_index] = average_flt(values, len);
    }
  }

  return retval;
}


flt_mat*
create_flt_vectors(const flt_mat *const in, const size_t H, const size_t L)
{
  flt_mat_ok(in);

  const size_t vector_length = H*H;
  const size_t vector_count = (in->width/(H*L)) * (in->height/(H*L)); 

  flt_mat *retval = malloc(sizeof(flt_mat));
  assert(retval != NULL);
  retval->width = vector_count;
  retval->height = vector_length;

  retval->matrix = malloc(vector_count * sizeof(flt*));
  assert(retval->matrix != NULL);

  const flt_mat *const scaled = scale_flt_mat(in, L);

  size_t which_vector=0;
  for (size_t x_index=0; x_index<scaled->width/H; x_index++) {
    for (size_t y_index=0; y_index<scaled->height/H; y_index++) {
      assert(which_vector < vector_count);
      retval->matrix[which_vector] = linearize_flt_mat(scaled, x_index, y_index, H);
      which_vector++;
    }
  }
  assert(which_vector == vector_count);

  return retval;
}


hex_mat*
create_hex_vectors(const hex_mat *const in, const size_t H, const size_t L)
{
  hex_mat_ok(in);

  const size_t vector_length = H*H;
  const size_t vector_count = (in->width/(H*L)) * (in->height/(H*L)); 

  hex_mat *retval = malloc(sizeof(hex_mat));
  assert(retval != NULL);
  retval->width = vector_count;
  retval->height = vector_length;

  retval->matrix = malloc(vector_count * sizeof(hex*));
  assert(retval->matrix != NULL);

  const hex_mat *const scaled = scale_hex_mat(in, L);

  size_t which_vector=0;
  for (size_t x_index=0; x_index<scaled->width/H; x_index++) {
    for (size_t y_index=0; y_index<scaled->height/H; y_index++) {
      assert(which_vector < vector_count);
      retval->matrix[which_vector] = linearize_hex_mat(scaled, x_index, y_index, H);
      which_vector++;
    }
  }
  assert(which_vector == vector_count);

  return retval;
}


void
mul_hi_lo_mat(const flt_mat *const in, const hex m,
	      flt_mat **const mulhi_out, flt_mat **const mullo_out)
{
  flt_mat *mulhi = malloc_flt_mat(in->width, in->height);
  assert(mulhi != NULL);
  *mulhi_out = mulhi;

  flt_mat *mullo = malloc_flt_mat(in->width, in->height);
  assert(mullo != NULL);
  *mullo_out = mullo;

  for (size_t x_index=0; x_index<in->width; x_index++) {
    for (size_t y_index=0; y_index<in->height; y_index++) {
      const mul_unit both = mul_hi_lo(in->matrix[x_index][y_index], m);
      mulhi->matrix[x_index][y_index] = (flt) both.hi;
      mullo->matrix[x_index][y_index] = (flt) both.lo;
    }
  }
}


void
print_flt_vectors(const flt_mat *const in, char * label)
{
  fprint_flt_vectors(stdout, in, label);
}

void
save_flt_vectors(char * filename, const flt_mat *const in, char * label)
{
  FILE * fp = fopen(filename, "w");
  assert(fp != NULL);
  fprint_flt_vectors(fp, in, label);
}



void
fprint_flt_vectors(FILE * stream, const flt_mat *const in, char * label)
{
  flt_mat_ok(in);

  for (size_t x_index=0; x_index<in->width; x_index++) {
    size_t y_index;
    fprintf(stream, "%s ", label);
    for (y_index=0; y_index<in->height-1; y_index++) {  
      fprintf(stream, "%lu:%f ", y_index+1, in->matrix[x_index][y_index]);
    }
    fprintf(stream, "%f\n", in->matrix[x_index][y_index]);
  }
}


void
print_hex_vectors(const hex_mat *const in)
{
  fprint_hex_vectors(stdout, in);
}

void
save_hex_vectors(char * filename, const hex_mat *const in)
{
  FILE * fp = fopen(filename, "w");
  assert(fp != NULL);
  fprint_hex_vectors(fp, in);
}

void
fprint_hex_vectors(FILE * stream, const hex_mat *const in)
{
  hex_mat_ok(in);

  for (size_t x_index=0; x_index<in->width; x_index++) {
    size_t y_index;
    for (y_index=0; y_index<in->height-1; y_index++) {  
      fprintf(stream, "%u ", in->matrix[x_index][y_index]);
    }
    fprintf(stream, "%u\n", in->matrix[x_index][y_index]);
  }
}


vectors*
create_vectors(const flt_mat *const in, const hex m, const size_t H, const size_t L) 
{
  flt_mat_ok(in);
  assert(in->width%(H*L) == 0);
  assert(in->height%(H*L) == 0);

  vectors *const retval = malloc(sizeof(vectors));
  assert(retval != NULL);

  flt_mat *mulhi_mat;
  flt_mat *mullo_mat;

  mul_hi_lo_mat(in, m, &mulhi_mat, &mullo_mat);
  
  retval->average = create_flt_vectors(in, H, L);
  retval->mulhi = create_flt_vectors(mulhi_mat, H, L);
  retval->mullo = create_flt_vectors(mullo_mat, H, L);

  return retval;
}
