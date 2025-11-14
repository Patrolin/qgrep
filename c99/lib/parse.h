#pragma once
#include "definitions.h"

bool str_continues_with(String a, intptr start, String b) {
  return str_equals(str_slice(a, start, start + (intptr)b.size), b);
}
bool str_starts_with(String str, String prefix) {
  if (str.size < prefix.size) {
    return false;
  }
  return str_equals(str_slice(str, 0, prefix.size), prefix);
}
bool str_ends_with(String str, String suffix) {
  if (str.size < suffix.size) {
    return false;
  }
  return str_equals(str_slice(str, str.size - suffix.size, str.size), suffix);
}
String trim_prefix(String str, String prefix) {
  return str_starts_with(str, prefix) ? str_slice(str, prefix.size, str.size) : str;
}
String trim_suffix(String str, String prefix) {
  return str_ends_with(str, prefix) ? str_slice(str, 0, str.size - prefix.size) : str;
}

// IWYU pragma: begin_exports
#include "parse_int.h"
// IWYU pragma: end_exports
