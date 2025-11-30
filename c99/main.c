// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FORMAT_FLOAT_SIZE 1 + 17 + 1 + 5 + 1
string shorten_float_string(f64 value, string value_string) {
  // preserve "+-inf", "nan"
  intptr size = intptr(value_string.size);
  if (size == 0) {
    printf("%s -> %s\n", value_string.ptr, value_string.ptr);
    return value_string;
  }
  bool is_negative = value_string.ptr[0] == '-';
  intptr start = is_negative || value_string.ptr[0] == '+' ? 1 : 0;
  if (value_string.ptr[start] < '0' || value_string.ptr[start] > '9') {
    printf("%s -> %s\n", value_string.ptr, value_string.ptr);
    return value_string;
  }
  // -- shorten float --
  char original[FORMAT_FLOAT_SIZE];
  memcpy(original, value_string.ptr, min(Size(size), sizeof(original)));
  original[size] = 0;
  //  find exponent
  intptr exponent_index = 0;
  while (exponent_index < size && original[exponent_index] != 'e') {
    exponent_index++;
  }
  // round up and trucate
  char shortened[FORMAT_FLOAT_SIZE];
  intptr significand_end = exponent_index;
  memcpy(shortened, original, FORMAT_FLOAT_SIZE);
  while (significand_end >= 1) {
    u8 carry = 1;
    for (intptr j = significand_end - 2; j >= 0; j--) {
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
  return value_string; /* TODO: alloc and return shortened */
}

/* NOTE: sign(1) + digits(17) + decimal_point(1) + signed_exponent(5) + null_terminator(1) */
void format_float_libc(f64 value) {
  char original[FORMAT_FLOAT_SIZE];
  int size = snprintf(original, sizeof(original), "%.17g", value);
  shorten_float_string(value, (string){original, Size(size)});
}

void main_multicore(u32 t) {
  if (single_core(t)) {
    // intptr end;
    // f64 x = parse_f64(string("0.5e1"), 0, &end);
    // printfln1(string("end: %"), intptr, end);
    format_float_libc(1.2e234);              // 1.2e+234
    format_float_libc(0.3);                  // 0.3
    format_float_libc(0.30000000000000004);  // 0.30000000000000004
    format_float_libc(NAN);
  }
}
