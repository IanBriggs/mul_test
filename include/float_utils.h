#ifndef FLOAT_SUM_H
#define FLOAT_SUM_H

#include <stdint.h>
#include <stddef.h>


#include "static_assert.h"

typedef float flt;
typedef uint32_t hex;
typedef uint64_t long_hex;;
static const size_t HEX_RAND_MAX = UINT32_MAX;

typedef struct _mul_unit {
  hex hi : sizeof(hex)*8;
  hex lo : sizeof(hex)*8;
} mul_unit;

STATIC_ASSERT(sizeof(flt) == sizeof(hex), 
	      "flt and hex must be the same size");
STATIC_ASSERT(sizeof(long_hex) == 2*sizeof(hex),
	      "long_hex must be twice the size of hex");
STATIC_ASSERT(sizeof(mul_unit) == sizeof(long_hex),
	      "mul_unit must be the same size as long_hex");

flt flt_sum(const flt *const float_array, const size_t length);
hex flt_to_hex(const flt in);
flt hex_to_flt(const hex in);
flt rand_real_range(const flt low, const flt high);
hex rand_hex_range(const hex low, const hex high);
hex ulps_between(const flt low, const flt high);
flt rand_ulp_range(const flt low, const flt high);
flt corrupt_flt(const flt in, const size_t high_index, const size_t low_index);
mul_unit mul_hi_lo(const flt x, const hex m) ;

#endif
