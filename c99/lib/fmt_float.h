#pragma once
#include "definitions.h"
#include "math.h"

// TODO: use qfloat lib instead

/* NOTE: f64 needs at most 17 (integer+fraction) digits: https://www.exploringbinary.com/number-of-digits-required-for-round-trip-conversions/
   = Math.ceil(explicitmantissa_bits*Math.log10(2))+1 */
/* NOTE: sign(1) + digits(17) + decimal_point(1) + signed_exponent(5) + null_terminator(1) */
#define sprint_size_f64(value) (Size(1) + 17 + 1 + 5 + 1)
#if HAS_CRT
  #include <math.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
void shorten_f64_string(f64 value, byte buffer[sprint_size_f64(0.0)], Size size) {
  assert(size < sprint_size_f64(0.0));
  // preserve "+-inf", "nan"
  if (size == 0) return;
  intptr start = buffer[0] == '-' || buffer[0] == '+' ? 1 : 0;
  if (buffer[start] < '0' || buffer[start] > '9') return;
  // find exponent
  intptr exponent_index = 0;
  while (exponent_index < size && buffer[exponent_index] != 'e') {
    exponent_index++;
  }
  // round up and trucate
  char shortened[sprint_size_f64(value)];
  memcpy(shortened, buffer, size);
  while (exponent_index > 1) {
    /* NOTE: here we produce a wrong result if it would overflow to a new digit,
       but then we discard it anyway, so we don't actually care */
    u8 carry = 1;
    for (intptr j = exponent_index - 2; j >= 0; j--) {
      char c = shortened[j];
      if (c < '0' || c > '9') continue;
      u8 digit = (c - '0') + carry;
      shortened[j] = '0' + (digit % 10);
      carry = digit >= 10 ? 1 : 0;
    }
    memcpy(shortened + exponent_index - 1, buffer + exponent_index, size - Size(exponent_index));
    shortened[size - 1] = '\0';
    // if valid transform, copy to original
    if (strtod(shortened, 0) != value) break;
    memcpy(buffer, shortened, size);
    exponent_index--;
    size--;
  }
  // truncate
  intptr significand_end = exponent_index;
  intptr exponent_end = intptr(size);
  memcpy(shortened, buffer, (size_t)exponent_index);
  while (significand_end > 1) {
    memcpy(shortened + significand_end - 1, buffer + exponent_index, size - Size(exponent_index));
    shortened[exponent_end - 1] = '\0';
    // if valid transform, update significand_end
    if (strtod(shortened, 0) != value) break;
    significand_end--;
    exponent_end--;
  }
  memcpy(buffer + significand_end, buffer + exponent_index, size - Size(exponent_index));
  buffer[exponent_end] = '\0';
}
void sprint_f64_libc(f64 value, byte *ptr_end) {
  byte *buffer = ptr_end - sprint_size_f64(0.0);
  int size = snprintf(buffer, sprint_size_f64(0.0), "%.17g", value);
  shorten_f64_string(value, buffer, Size(size));
}
#endif

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
