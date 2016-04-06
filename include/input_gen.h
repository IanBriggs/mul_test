#ifndef INPUT_GEN_H
#define INPUT_GEN_H

#include <stddef.h>


#include "float_utils.h"
#include "flt_mat.h"

typedef flt (*flt_function)(flt, flt);

flt sine_2d(const flt x, const flt y);

flt_mat* gen(const size_t width, const size_t height, 
	    const flt x_start, const flt x_end,
	    const flt y_start, const flt y_end,
	    const flt_function func);

flt_mat* gen_sine_2d(const size_t width, const size_t height, 
		    const flt x_start, const flt x_end,
		    const flt y_start, const flt y_end);

flt_mat* gen_square(const size_t width, 
		   const flt start, const flt end, 
		   const flt_function func);

flt_mat* gen_square_sine_2d(const size_t width, 
			   const flt start, const flt end);

flt_mat* corrupt_flt_mat(const flt_mat *const in, 
			 const size_t errors_per, const size_t LH,
			 const size_t high_index, const size_t low_index);

#endif
