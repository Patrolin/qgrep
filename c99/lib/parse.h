#pragma once
#include "definitions.h"

bool str_continues_with(string a, intptr start, string b) {
  return str_equals(str_slice(a, start, start + intptr(b.size)), b);
}
bool str_starts_with(string str, string prefix) {
  if (expect_unlikely(str.size < prefix.size)) {
    return false;
  }
  return str_equals(str_slice(str, 0, prefix.size), prefix);
}
bool str_ends_with(string str, string suffix) {
  if (expect_unlikely(str.size < suffix.size)) {
    return false;
  }
  return str_equals(str_slice(str, str.size - suffix.size, str.size), suffix);
}
string trim_prefix(string str, string prefix) {
  return str_starts_with(str, prefix) ? str_slice(str, prefix.size, str.size) : str;
}
string trim_suffix(string str, string prefix) {
  return str_ends_with(str, prefix) ? str_slice(str, 0, str.size - prefix.size) : str;
}

// IWYU pragma: begin_exports
#include "parse_int.h"
#include "parse_float.h"
// IWYU pragma: end_exports
