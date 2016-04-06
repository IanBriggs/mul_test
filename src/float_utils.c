#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "float_utils.h"




static void
swap(flt *const a, flt *const b)
{
  assert(a != NULL);
  assert(b != NULL);

  flt temp = *a;
  *a = *b;
  *b = temp;
}


static flt
naive_sum(const flt *const float_array, const size_t length)
{
  assert(float_array != NULL);

  flt sum = 0.0;
  for (size_t i = 0; i < length; i++) {
    sum += float_array[i];
  }

  return sum;
}


flt
flt_sum(const flt *const float_array, const size_t length)
{
  assert(float_array != NULL);

  flt *const partials = malloc(length * sizeof(flt));
  assert(partials != NULL);
  size_t partials_length = 0;

  for (size_t xi = 0; xi < length; xi++) {
    flt x = float_array[xi];
    size_t i = 0;

    for (size_t yi = 0; yi < partials_length; yi++) {
      flt y = partials[yi];

      if (fabs(x) < fabs(y)) {
	swap(&x, &y);
      }

      const flt hi = x + y;
      const flt lo =  y - (volatile flt)(hi - x);

      if ((lo != 0.0) && (lo != -0.0)) {
	assert(i < length);
	partials[i] = lo;
	i++;
      }

      x = hi;
    }

    assert(i < length);
    partials[i] = x;
    partials_length = i+1;
  }

  const flt sum = naive_sum(partials, partials_length);
  free(partials);

  return sum;
}


hex
flt_to_hex(const flt in)
{
  hex retval;
  memcpy(&retval, &in, sizeof(in));
  return retval;
}


flt
hex_to_flt(const hex in)
{
  flt retval;
  memcpy(&retval, &in, sizeof(in));
  return retval;
}


static hex
rand_32_bit()
{
  static size_t bit_index = 31;
  static hex rand_bitfield;

  if (bit_index == 31) {
    rand_bitfield = random();
    bit_index = 0;
  }

  hex r = random();
  r |= ((rand_bitfield >> bit_index) & 0x1) << 31;
  bit_index++;

  return r;
}


flt
rand_real_range(const flt low, const flt high)
{
  assert(!isnan(low));
  assert(!isnan(high));
  assert(low <= high);

  const flt r = ((flt) rand_32_bit()) / (flt) HEX_RAND_MAX;
  const flt diff = high - low;
  const flt rr = r * diff;
  const flt retval = low + rr;

  assert(retval >= low);
  assert(retval <= high);
  return retval;
}


static hex
rand_hex()
{
  if (sizeof(hex) == 4) {
    return rand_32_bit();
  } else if (sizeof(hex) == 8) {
    return rand_32_bit() | (rand_32_bit() << 32);
  } else {
    assert(0);
  }
}


hex
rand_hex_range(const hex low, const hex high)
{
  assert(low <= high);
  assert(high <= HEX_RAND_MAX);

  const hex width = high - low + 1; // +1 so we are inclusive
  const hex width_repeats = HEX_RAND_MAX / width;
  const hex retry_if_under = width_repeats * width;

  hex r;
  do {
    r = rand_hex();
  } while (r >= retry_if_under);
  const hex retval = low + r%width;

  assert(retval >= low);
  assert(retval <= high);
  return retval;
}


static flt
rand_pos_ulp_range(const flt low, const flt high)
{
  assert(!isnan(low));
  assert(!isnan(high));
  assert(low <= high);
  assert(low >= 0.0);

  const hex low_hex = flt_to_hex(low);
  const hex high_hex = flt_to_hex(high);

  const hex r = rand_hex_range(low_hex, high_hex);
  const flt retval = hex_to_flt(r);

  assert(retval >= low);
  assert(retval <= high);
  return retval;
}


static hex
ulps_between_positive(const flt low, const flt high)
{
  assert(!isnan(low));
  assert(!isnan(high));
  assert(low <= high);
  assert(low >= 0.0);

  const hex low_hex = flt_to_hex(low);
  const hex high_hex = flt_to_hex(high);

  return high_hex - low_hex + 1;  
}


hex
ulps_between(const flt low, const flt high)
{
  assert(!isnan(low));
  assert(!isnan(high));
  assert(low <= high);

  hex retval;
  if (low >= 0) {
    retval = ulps_between_positive(low, high);
  } else if(high <= 0) {
    retval =  ulps_between_positive(-high, -low);
  } else {
    const hex neg_part = ulps_between_positive(0.0, -low);
    const hex pos_part = ulps_between_positive(0.0, high);
    retval = neg_part + pos_part - 1;
  }
  
  return retval;
}


flt
rand_ulp_range(const flt low, const flt high)
{
  assert(!isnan(low));
  assert(!isnan(high));
  assert(low <= high);

  flt retval;
  
  if (low >= 0) { // Both positive
    retval = rand_pos_ulp_range(low, high);
  } else if (high <= 0) { // Both negative
    retval = -rand_pos_ulp_range(-high, -low);
  } else { // Mixed
    const hex neg_ulps = ulps_between(low, 0.0);
    const hex pos_ulps = ulps_between(0.0, high);
    const hex total_ulps = neg_ulps + pos_ulps - 1;
    
    if (rand_hex_range(0, total_ulps) <= neg_ulps) {
      retval = -rand_pos_ulp_range(0.0, -low);
    } else {
      retval =  rand_pos_ulp_range(0.0, high);
    }
  }

  assert(retval >= low);
  assert(retval <= high);
  return retval;
}


flt
corrupt_flt(const flt in, const size_t high_index, const size_t low_index) 
{
  assert(low_index >= 0);
  assert(high_index >= low_index);
  assert((sizeof(flt)*8)-1 >= high_index);

  const size_t target_index = rand_hex_range(low_index, high_index);
  hex in_hex = flt_to_hex(in);
  in_hex  ^= 1 << target_index;
  const flt retval = hex_to_flt(in_hex);

  assert(flt_to_hex(retval) != flt_to_hex(in));
  return retval;
}


mul_unit
mul_hi_lo(const flt x, const hex m) 
{
  const long_hex both = (long_hex) m * (long_hex) flt_to_hex(x);
  mul_unit retval;
  memcpy(&retval, &both, sizeof(long_hex));

  return retval;
}
