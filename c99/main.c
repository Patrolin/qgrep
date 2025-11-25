// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"
#include <stdio.h>
#include <string.h>
_Check_return_ _ACRTIMP double __cdecl nan(_In_ char const* _X);

/* NOTE: sign(1) + digits(17) + decimal_point(1) + signed_exponent(5) + null_terminator(1) */
#define FORMAT_FLOAT_SIZE 1 + 17 + 1 + 5 + 1
void format_float(f64 value) {
  char original[FORMAT_FLOAT_SIZE];
  int size = snprintf(original, sizeof(original), "%.17g", value);
  /* TODO: if (isnan(value) || isinf(value)) return original; */
  // find exponent
  int exponent_index = 0;
  while (exponent_index < size && original[exponent_index] != 'e') {
    exponent_index++;
  }
  // round up and trucate
  char shortened[FORMAT_FLOAT_SIZE];
  int significand_end = exponent_index;
  memcpy(shortened, original, FORMAT_FLOAT_SIZE);
  while (significand_end >= 1) {
    u8 carry = 1;
    for (int j = significand_end - 2; j >= 0; j--) {
      char c = shortened[j];
      if (c == '.' || c == '-') continue;
      u8 digit = (c - '0') + carry;
      shortened[j] = '0' + (digit % 10);
      carry = digit >= 10 ? 1 : 0;
    }
    memcpy(shortened + significand_end - 1, original + exponent_index, (size_t)(size - exponent_index + 1));
    /* if valid transform, copy to original */
    if (strtod(shortened, 0) == value) {
      size--;
      exponent_index--;
      memcpy(original, shortened, (size_t)(size + 1));
      significand_end--;
      continue;
    }
    break;
  }
  // truncate
  memcpy(shortened, original, (size_t)exponent_index);
  while (significand_end >= 1) {
    memcpy(shortened + significand_end - 1, original + exponent_index, (size_t)(size - exponent_index + 1));
    /* if valid transform, keep in shortened */
    if (strtod(shortened, 0) == value) {
      significand_end--;
      continue;
    }
    break;
  }
  // output
  shortened[significand_end - 1] = original[significand_end - 1];
  memcpy(shortened + significand_end, original + exponent_index, (size_t)(size - exponent_index + 1));
  printf("%.17g -> %s\n", value, shortened);
}

void main_multicore(u32 t) {
  if (single_core(t)) {
    // intptr end;
    // f64 x = parse_f64(string("0.5e1"), 0, &end);
    // printfln1(string("end: %"), intptr, end);

    format_float(1.2e234);              // 1.2e+234
    format_float(0.3);                  // 0.3
    format_float(0.30000000000000004);  // 0.30000000000000004
  }
}
