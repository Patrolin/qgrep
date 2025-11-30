#pragma once
#include "definitions.h"
#include "parse.h"
#include "parse_int.h"

f64 parse_fraction64_decimal(string str, intptr start, intptr* _Nonnull end) {
  // find end
  intptr i = start;
  while (i < str.size) {
    byte digit = str.ptr[i] - '0';
    if (expect_unlikely(digit > 10)) {
      break;
    }
    i++;
  }
  *end = i;
  // compute result from smallest to largest
  f64 result = 0.0;
  while (i > start) {
    i--;
    byte digit = str.ptr[i] - '0';
    result = result * 0.1 + (f64)digit;
  }
  return result * 0.1;
}
/* TODO: proper float print...
void print_float(string name, f64 value) {
  u64 hex_value = bitcast(value, f64, u64);
  printfln2(string("%.sign: %"), string, name, u64, hex_value >> 63);
  u64 exponent_to_print = (hex_value >> 52) & 0x7ff;
  if (expect_unlikely(exponent_to_print == 0)) {
    printfln1(string("%.exponent: denormal"), string, name);
  } else {
    printfln2(string("%.exponent: %"), string, name, i64, i64(exponent_to_print) - 1023);
  }
  printfln2(string("%.mantissa: %"), string, name, uhex_pad, hex_value & 0x000fffffffffffff);
  printfln2(string("%.mantissa_log2: %"), string, name, i64, 12 - i64(count_leading_zeros(u64, hex_value & 0x000fffffffffffff)));
}*/
f64 parse_f64_decimal(string str, intptr start, intptr* _Nonnull end) {
  intptr i = start;
  // sign
  bool negative = i < str.size && str.ptr[i] == '-';
  if (expect_small(negative)) {
    i++;
  }
  // mantissa
  f64 mantissa_f64 = (f64)parse_u64_decimal(str, i, &i);
  if (expect_small(i < str.size && str.ptr[i] == '.')) {
    mantissa_f64 += parse_fraction64_decimal(str, i + 1, &i);
  }
  // exponent
  f64 exponent_f64 = 1;
  bool exponent_is_negative = false;
  if (expect_small(i < str.size && str.ptr[i] == 'e')) {
    i++;
    u64 base10_exponent = parse_u64_decimal(str, i, &i);
    exponent_is_negative = base10_exponent < 0;
    base10_exponent = exponent_is_negative ? -base10_exponent : base10_exponent;
    f64 next_pow10_step = 10;
    while (base10_exponent > 0) {
      if (expect_small((base10_exponent & 1) == 1)) {
        exponent_f64 = exponent_f64 * next_pow10_step;
      }
      next_pow10_step = next_pow10_step * next_pow10_step;
      base10_exponent = base10_exponent >> 1;
    }
  }
  // compose float
  f64 value = exponent_is_negative ? mantissa_f64 / exponent_f64 : mantissa_f64 * exponent_f64;
  value = negative ? -value : value;
  // print float parts
  *end = i;
  return 0.0;
}
f64 parse_f64(string str, intptr start, intptr* _Nonnull end) {
  if (expect_unlikely(str_continues_with(str, start, string("0x")))) {
    u64 hex_value = parse_u64_hex(str, start, end);
    return bitcast(hex_value, u64, f64);
  } else {
    return parse_f64_decimal(str, start, end);
  }
}
