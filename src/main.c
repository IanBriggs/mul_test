#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "float_utils.h"
#include "input_gen.h"
#include "vector_creation.h"

flt k;
flt pi;

int
main(int argc, char **argv)
{
  assert(argc == 5);
  size_t L = atoi(argv[1]);		// BAD PRACTICE
  size_t H = atoi(argv[2]);		// BAD PRACTICE
  size_t errors = atoi(argv[3]);	// BAD PRACTICE
  k = strtod(argv[4], NULL);	        // BAD PRACTICE

  size_t width = L*H*50;
  hex m = 214013;
  pi = 3.14159265358979323846264338327950288; // For some reason this is not coming in with math.h

  srandom(time(NULL));

  flt_mat *clean = gen_square_sine_2d(width, 0, 1);
  save_flt_mat("clean_matrix.txt", clean);
  vectors *clean_vecs = create_vectors(clean, m, H, L);

  save_flt_vectors("clean_average.vec", clean_vecs->average);
  save_hex_vectors("clean_mulhi.vec", clean_vecs->mulhi);
  save_hex_vectors("clean_mullo.vec", clean_vecs->mullo);



  flt_mat *dirty = corrupt_flt_mat(clean, errors, L*H, 31, 0);
  save_flt_mat("dirty_matrix.txt", dirty);

  vectors *dirty_vecs = create_vectors(dirty, m, H, L);
  save_flt_vectors("dirty_average.vec", dirty_vecs->average);
  save_hex_vectors("dirty_mullhi.vec", dirty_vecs->mulhi);
  save_hex_vectors("dirty_mullo.vec", dirty_vecs->mullo);
  

  return 0;
}
