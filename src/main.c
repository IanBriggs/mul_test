#include <stdio.h>

#include "float_utils.h"
#include "input_gen.h"
#include "vector_creation.h"

int
main()
{
  size_t L = 3;
  size_t H = 3;
  size_t width = L*H*50;
  hex m = 214013;
  flt four_pi = 12.566370614359172;

  srandom(time(NULL));

  flt_mat *clean = gen_square_sine_2d(width, 0, four_pi);
  vectors *clean_vecs = create_vectors(clean, m, H, L);
  printf("CLEAN:\n");
  printf("AVERAGE:\n");
  print_flt_vectors(clean_vecs->average);

  printf("\n\nMULHI:\n");
  print_hex_vectors(clean_vecs->mulhi);

  printf("\n\nMULLO:\n");
  print_hex_vectors(clean_vecs->mullo);


  flt_mat *dirty = corrupt_flt_mat(clean, 1, L*H, 31, 0);
  vectors *dirty_vecs = create_vectors(dirty, m, H, L);
  printf("\n\n\n\nDIRTY:\n");
  printf("AVERAGE:\n");
  print_flt_vectors(dirty_vecs->average);

  printf("\n\nMULHI:\n");
  print_hex_vectors(dirty_vecs->mulhi);

  printf("\n\nMULLO:\n");
  print_hex_vectors(dirty_vecs->mullo);
  

  return 0;
}
