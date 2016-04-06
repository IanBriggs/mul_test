#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>


#include "input_gen.h"


flt
sine_2d(const flt x, const flt y)
{
  static flt scale = NAN;
  if (isnan(scale)) {
    scale = rand_real_range(-1.0e20, 1.0e20);
  }
  return scale * sin(x + y);
}




flt_mat*
gen(const size_t width, const size_t height, 
    const flt x_start, const flt x_end,
    const flt y_start, const flt y_end,
    const flt_function func)
{
  assert(x_start < x_end);
  assert(y_start < y_end);
  assert(ulps_between(x_start, x_end) >= width);
  assert(ulps_between(y_start, y_end) >= height);

  flt_mat *const retval = malloc_flt_mat(width, height);

  const flt y_step = (y_end - y_start) / height;
  const flt x_step = (x_end - x_start) / width;
  for(size_t y_index=0; y_index<height; y_index++) {
    for (size_t x_index=0; x_index<width; x_index++) {
      retval->matrix[x_index][y_index] = func(x_start + x_step*x_index,
					      y_start + y_step*y_index);
    }
  }

  flt_mat_ok(retval);
  return retval;
}


flt_mat*
gen_sine_2d(const size_t width, const size_t height, 
	    const flt x_start, const flt x_end,
	    const flt y_start, const flt y_end)
{
  return gen(width, height, x_start, x_end, y_start, y_end, &sine_2d);
}


flt_mat*
gen_square(const size_t width, 
	   const flt start, const flt end, 
	   const flt_function func)
{
  return gen(width, width, start, end, start, end, func);
}


flt_mat*
gen_square_sine_2d(const size_t width, 
		   const flt start, const flt end)
{
  return gen_sine_2d(width, width, start, end, start, end);
}


flt_mat* 
corrupt_flt_mat(const flt_mat *const in, 
		const size_t errors_per, const size_t LH,
		const size_t high_index, const size_t low_index)
{
  flt_mat_ok(in);
  assert(high_index >= low_index);
  assert(LH >= 1);
  assert(errors_per >= 1);
  assert(in->width % LH == 0);
  assert(in->height % LH == 0);

  flt_mat *const retval = copy_flt_mat(in);

  for (size_t x=0; x<in->width/LH; x++) {
    for (size_t y=0; y<in->height/LH; y++) {
      for (size_t times=0; times<errors_per; times++) {
	const size_t x_ind = rand_hex_range(LH*x, LH*(x+1)-1);
	const size_t y_ind = rand_hex_range(LH*y, LH*(y+1)-1);
	assert(x_ind < in->width);
	assert(y_ind < in->height);
	retval->matrix[x_ind][y_ind] = corrupt_flt(retval->matrix[x_ind][y_ind], 
						   high_index, low_index);
      }
    }
  }

  flt_mat_ok(retval);
  return retval;
}
