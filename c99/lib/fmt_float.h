#pragma once
#include "definitions.h"
#include "math.h"

/* TODO: f64 need at most 17 (integer+fraction) digits: https://www.exploringbinary.com/number-of-digits-required-for-round-trip-conversions/
   = Math.ceil(explicitmantissa_bits*Math.log10(2))+1
// */

#define BITS_f64 64
#define EXPLICIT_MANTISSA_BITS_f64 52
#define IMPLICIT_MANTISSA_BITS_f64 (EXPLICIT_MANTISSA_BITS_f64 + 1)
#define EXPONENT_BITS_f64 = (BITS_f64 - IMPLICIT_MANTISSA_BITS_f64)

// "augmented arithmetic operations"
void augmented_mul(f64 a, f64 b, f64 *result, f64 *error) {
  f64 p = a * b;
  *error = fma(a, b, -p); /* NOTE: fma() has infinite precision for `a*b` */
  *result = p;
}
void augmented_fma(f64 x, f64 y, f64 z, f64 *result, f64 *error) {
  f64 p = fma(x, y, z);
  *error = fma(x, y, z - p);
  *result = p;
}
void augmented_add(f64 a, f64 b, f64 *result, f64 *error) {
#if 0
  augmented_fma(1.0, a, b, result, error);
#else
  f64 s = a + b;
  f64 bb = s - a;
  *error = (a - (s - bb)) + (b - bb);
  *result = s;
#endif
}
void augmented_add_fast(f64 a, f64 b, f64 *result, f64 *error) {
  assert(fabs(a) >= fabs(b));
  f64 s = a + b;
  *error = b - (s - a);
  *result = s;
}
