#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "float_utils.h"
#include "input_gen.h"
#include "vector_creation.h"

flt k;
flt pi;

void
process_flow(flt_mat *in, hex m, size_t H, size_t L, char * prefix, char * label)
{
  char buf[1024];
  sprintf(buf, "%s_matrix.txt", prefix);
  save_flt_mat(buf, in);

  vectors * vecs = create_vectors(in, m, H, L);

  sprintf(buf, "%s_average.vec", prefix);
  save_flt_vectors(buf, vecs->average, label);

  sprintf(buf, "%s_mulhi.vec", prefix);
  save_flt_vectors(buf, vecs->mulhi, label);

  sprintf(buf, "%s_mullo.vec", prefix);
  save_flt_vectors(buf, vecs->mullo, label);
}


int
main(int argc, char **argv)
{
  assert(argc == 5);
  size_t H = atoi(argv[1]);		// BAD PRACTICE
  size_t L = atoi(argv[2]);		// BAD PRACTICE
  size_t errors = atoi(argv[3]);	// BAD PRACTICE
  k = strtod(argv[4], NULL);	        // BAD PRACTICE

  size_t width = L*H*50;
  hex m = 214013;
  pi = 3.14159265358979323846264338327950288; // For some reason this is not coming in with math.h

  srandom(time(NULL));

  flt_mat *clean = gen_square_sine_2d(width, 0, 1);
  process_flow(clean, m, H, L, "clean", "1");

  flt_mat *expo = corrupt_flt_mat(clean, errors, L*H, 30, 23);
  process_flow(expo, m, H, L, "exponent_corruption", "-1");

  flt_mat *umant = corrupt_flt_mat(clean, errors, L*H, 22, 12);
  process_flow(umant, m, H, L, "upper_mantissa_corruption", "-1");

  flt_mat *lmant = corrupt_flt_mat(clean, errors, L*H, 11, 0);
  process_flow(lmant, m, H, L, "lower_mantissa_corruption", "-1");

  return 0;
}
