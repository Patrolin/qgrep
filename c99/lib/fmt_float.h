#pragma once
#include "definitions.h"
#include "math.h"

#define BITS_f64 64
#define SIGNIFICAND_EXPLICIT_BITS_f64 52
#define EXPONENT_BITS_f64 = (BITS_f64 - SIGNIFICAND_EXPLICIT_BITS_f64)

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
void augmented_add_fast(double a, double b, double *result, double *error) {
  assert(abs(a) >= abs(b));
  f64 s = a + b;
  *error = b - (s - a);
  *result = s;
}
