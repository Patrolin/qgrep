#pragma once
#include "definitions.h"
#include "parse.h"

u64 parse_u64_hex(string str, intptr start, intptr* _Nonnull end) {
  u64 result = 0;
  intptr i = start;
  while (i < str.size) {
    byte c = str.ptr[i];
    byte digit = 16;
    switch (c) {
      case '0' ... '9':
        digit = c - '0';
        break;
      case 'a' ... 'f':
        digit = c - 'a' + 10;
        break;
      case 'A' ... 'F':
        digit = c - 'A' + 10;
        break;
    }
    u64 new_result;
    bool did_overflow = mul_overflow(result, 16, &new_result);
    did_overflow |= add_overflow(new_result, u64(digit), &new_result);
    if (expect_unlikely(digit >= 16 || did_overflow)) {
      break;
    }
    result = new_result;
    i++;
  }
  *end = i;
  return result;
}
u64 parse_u64_decimal(string str, intptr start, intptr* _Nonnull end) {
  u64 result = 0;
  intptr i = start;
  while (i < str.size) {
    byte digit = str.ptr[i] - '0';
    u64 new_result;
    bool did_overflow = mul_overflow(result, 10, &new_result);
    did_overflow |= add_overflow(new_result, u64(digit), &new_result);
    if (expect_unlikely(digit >= 10 || new_result < result)) {
      break;
    }
    result = new_result;
    i++;
  }
  *end = i;
  return result;
}
u64 parse_u64(string str, intptr start, intptr* _Nonnull end) {
  if (expect_unlikely(str_continues_with(str, start, string("0x")))) {
    return parse_u64_hex(str, start + 2, end);
  } else {
    return parse_u64_decimal(str, start, end);
  }
}
// TODO: parse_i64(), ...
