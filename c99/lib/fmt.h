#pragma once
#include "definitions.h"
#include "math.h"
#include "os.h"
#include "process.h" /* IWYU pragma: keep */

// sprint()
#define sprint_size_String(value) (value.size)
intptr sprint_String(String str, byte* buffer_end) {
  byte* buffer = buffer_end - str.size;
  for (intptr i = 0; i < str.size; i++) {
    buffer[i] = str.ptr[i];
  }
  return str.size;
}

/* NOTE: log10_ceil(max(type)) == log10_ceil(pow(2, bits) - 1) */
#define sprint_size_u64(value) (20)
#define sprint_size_u32(value) (10)
#define sprint_size_u16(value) (5)
#define sprint_size_u8(value) (3)
intptr sprint_u64(u64 value, byte* buffer_end) {
  intptr i = 0;
  do {
    intptr digit = value % 10;
    value = value / 10;
    buffer_end[--i] = '0' + (byte)digit;
  } while (value != 0);
  return -i;
}
intptr sprint_u32(u32 value, byte* buffer_end) {
  return sprint_u64(value, buffer_end);
}
intptr sprint_u16(u16 value, byte* buffer_end) {
  return sprint_u64(value, buffer_end);
}
intptr sprint_u8(u8 value, byte* buffer_end) {
  return sprint_u64(value, buffer_end);
}

/* NOTE: log10_ceil(-min(type)) + 1 == log10_ceil(pow(2, bits-1)) + 1 */
#define sprint_size_i64(value) (20)
#define sprint_size_i32(value) (11)
#define sprint_size_i16(value) (6)
#define sprint_size_i8(value) (4)
intptr sprint_i64(i64 value, byte* buffer_end) {
  u64 value_abs = (u64)abs(i64, value);
  intptr size = sprint_u64(value_abs, buffer_end);
  if (value < 0) {
    *(buffer_end - size - 1) = '-';
    size += 1;
  }
  return size;
}
intptr sprint_i32(i32 value, byte* buffer_end) {
  u64 value_abs = (u64)abs(i32, value);
  intptr size = sprint_u64(value_abs, buffer_end);
  if (value < 0) {
    *(buffer_end - size - 1) = '-';
    size += 1;
  }
  return size;
}
intptr sprint_i16(i16 value, byte* buffer_end) {
  u64 value_abs = (u64)abs(i16, value);
  intptr size = sprint_u64(value_abs, buffer_end);
  if (value < 0) {
    *(buffer_end - size - 1) = '-';
    size += 1;
  }
  return size;
}
intptr sprint_i8(i8 value, byte* buffer_end) {
  u64 value_abs = (u64)abs(i8, value);
  intptr size = sprint_u64(value_abs, buffer_end);
  if (value < 0) {
    *(buffer_end - size - 1) = '-';
    size += 1;
  }
  return size;
}

#if ARCH_IS_64BIT
  #define sprint_size_uintptr(value) sprint_size_u64(value)
intptr sprint_uintptr(uintptr value, byte* buffer_end) {
  return sprint_u64(value, buffer_end);
}
  #define sprint_size_intptr(value) sprint_size_i64(value)
intptr sprint_intptr(intptr value, byte* buffer_end) {
  return sprint_i64(value, buffer_end);
}
#elif ARCH_IS_32BIT
  #define sprint_size_uintptr(value) sprint_size_u32(value)
intptr sprint_uintptr(uintptr value, byte* buffer_end) {
  return sprint_u32(value, buffer_end);
}
  #define sprint_size_intptr(value) sprint_size_i32(value)
intptr sprint_intptr(intptr value, byte* buffer_end) {
  return sprint_i32(value, buffer_end);
}
#endif

// sprintf()
#define sprintf1(ptr_end, format, t1, v1) ({                                          \
  String _autogen_format = format;                                                    \
  intptr _autogen_size = 0;                                                           \
  intptr _autogen_i = _autogen_format.size;                                           \
  intptr _autogen_j = _autogen_i;                                                     \
  while (_autogen_i > 0) {                                                            \
    _autogen_i--;                                                                     \
    if (_autogen_format.ptr[_autogen_i] == '%') {                                     \
      String _autogen_after = str_slice(_autogen_format, _autogen_i + 1, _autogen_j); \
      _autogen_size += sprint_String(_autogen_after, ptr_end);                        \
      _autogen_j = _autogen_i;                                                        \
      _autogen_size += CONCAT(sprint_, t1)(v1, ptr_end - _autogen_size);              \
      break;                                                                          \
    }                                                                                 \
  }                                                                                   \
  if (_autogen_j > 0) {                                                               \
    String _autogen_before = str_slice(_autogen_format, 0, _autogen_j);               \
    _autogen_size += sprint_String(_autogen_before, ptr_end - _autogen_size);         \
  }                                                                                   \
  _autogen_size;                                                                      \
})
#define sprintf2(ptr_end, format, t1, v1, t2, v2) ({                                  \
  String _autogen_format = format;                                                    \
  intptr _autogen_size = 0;                                                           \
  intptr _autogen_i = _autogen_format.size;                                           \
  intptr _autogen_j = _autogen_i;                                                     \
  while (_autogen_i > 0) {                                                            \
    _autogen_i--;                                                                     \
    if (_autogen_format.ptr[_autogen_i] == '%') {                                     \
      String _autogen_after = str_slice(_autogen_format, _autogen_i + 1, _autogen_j); \
      _autogen_size += sprint_String(_autogen_after, ptr_end);                        \
      _autogen_j = _autogen_i;                                                        \
      _autogen_size += CONCAT(sprint_, t2)(v2, ptr_end - _autogen_size);              \
      break;                                                                          \
    }                                                                                 \
  }                                                                                   \
  while (_autogen_i > 0) {                                                            \
    _autogen_i--;                                                                     \
    if (_autogen_format.ptr[_autogen_i] == '%') {                                     \
      String _autogen_after = str_slice(_autogen_format, _autogen_i + 1, _autogen_j); \
      _autogen_size += sprint_String(_autogen_after, ptr_end - _autogen_size);        \
      _autogen_j = _autogen_i;                                                        \
      _autogen_size += CONCAT(sprint_, t1)(v1, ptr_end - _autogen_size);              \
      break;                                                                          \
    }                                                                                 \
  }                                                                                   \
  if (_autogen_j > 0) {                                                               \
    String _autogen_before = str_slice(_autogen_format, 0, _autogen_j);               \
    _autogen_size += sprint_String(_autogen_before, ptr_end - _autogen_size);         \
  }                                                                                   \
  _autogen_size;                                                                      \
})
#define sprintf3(ptr_end, format, t1, v1, t2, v2, t3, v3) ({                          \
  String _autogen_format = format;                                                    \
  intptr _autogen_size = 0;                                                           \
  intptr _autogen_i = _autogen_format.size;                                           \
  intptr _autogen_j = _autogen_i;                                                     \
  while (_autogen_i > 0) {                                                            \
    _autogen_i--;                                                                     \
    if (_autogen_format.ptr[_autogen_i] == '%') {                                     \
      String _autogen_after = str_slice(_autogen_format, _autogen_i + 1, _autogen_j); \
      _autogen_size += sprint_String(_autogen_after, ptr_end);                        \
      _autogen_j = _autogen_i;                                                        \
      _autogen_size += CONCAT(sprint_, t3)(v3, ptr_end - _autogen_size);              \
      break;                                                                          \
    }                                                                                 \
  }                                                                                   \
  while (_autogen_i > 0) {                                                            \
    _autogen_i--;                                                                     \
    if (_autogen_format.ptr[_autogen_i] == '%') {                                     \
      String _autogen_after = str_slice(_autogen_format, _autogen_i + 1, _autogen_j); \
      _autogen_size += sprint_String(_autogen_after, ptr_end - _autogen_size);        \
      _autogen_j = _autogen_i;                                                        \
      _autogen_size += CONCAT(sprint_, t2)(v2, ptr_end - _autogen_size);              \
      break;                                                                          \
    }                                                                                 \
  }                                                                                   \
  while (_autogen_i > 0) {                                                            \
    _autogen_i--;                                                                     \
    if (_autogen_format.ptr[_autogen_i] == '%') {                                     \
      String _autogen_after = str_slice(_autogen_format, _autogen_i + 1, _autogen_j); \
      _autogen_size += sprint_String(_autogen_after, ptr_end - _autogen_size);        \
      _autogen_j = _autogen_i;                                                        \
      _autogen_size += CONCAT(sprint_, t1)(v1, ptr_end - _autogen_size);              \
      break;                                                                          \
    }                                                                                 \
  }                                                                                   \
  if (_autogen_j > 0) {                                                               \
    String _autogen_before = str_slice(_autogen_format, 0, _autogen_j);               \
    _autogen_size += sprint_String(_autogen_before, ptr_end - _autogen_size);         \
  }                                                                                   \
  _autogen_size;                                                                      \
})

// assert()
#define assert(condition) ({                                                                                      \
  if (!(condition)) {                                                                                             \
    String format = string("%:% %");                                                                              \
    String file = string(__FILE__);                                                                               \
    intptr line = __LINE__;                                                                                       \
    String condition_str = string(#condition "\n");                                                               \
    intptr _autogen_max_size = sprint_size_String(format) + sprint_size_String(file);                             \
    _autogen_max_size += sprint_size_intptr(line) + sprint_size_String(condition_str);                            \
                                                                                                                  \
    byte _autogen_buffer[_autogen_max_size];                                                                      \
    byte* _autogen_ptr_end = &_autogen_buffer[_autogen_max_size];                                                 \
    intptr _autogen_size = sprintf3(_autogen_ptr_end, format, String, file, intptr, line, String, condition_str); \
    String _autogen_error = {_autogen_ptr_end - _autogen_size, _autogen_size};                                    \
    fprint(STDERR, _autogen_error);                                                                               \
    abort();                                                                                                      \
  }                                                                                                               \
})
// #define assertf(condition, t1, v1)

// fprint()
void fprint(FileHandle file, String str) {
#if OS_WINDOWS
  DWORD bytes_written;
  DWORD chars_to_write = downcast(intptr, str.size, DWORD);
  WriteFile(file, str.ptr, chars_to_write, &bytes_written, 0);
  assert(bytes_written == str.size);
#elif OS_LINUX
  intptr bytes_written = write(file, str.ptr, str.size);
  assert(bytes_written == str.size);
#else
  assert(false);
#endif
}

// print()
void print_String(String str) {
  fprint(STDOUT, str);
}
#define print_copy(t1, v1) ({                                       \
  intptr _autogen_max_size = CONCAT(sprint_size_, t1)(v1);          \
  byte _autogen_buffer[_autogen_max_size];                          \
  byte* _autogen_ptr_end = &_autogen_buffer[_autogen_max_size];     \
                                                                    \
  intptr _autogen_size = CONCAT(sprint_, t1)(v1, _autogen_ptr_end); \
  String _autogen_msg = {_autogen_ptr_end - size, size};            \
  print_String(_autogen_msg);                                       \
})
#define print(t1, v1) IF(IS_STRING(t1), print_String(v1), print_copy(t1, v1))
#define println(t1, v1) ({                                                          \
  intptr _autogen_max_size = CONCAT(sprint_size_, t1)(v1) + 1;                      \
  byte _autogen_buffer[_autogen_max_size];                                          \
  byte* _autogen_ptr_end = &_autogen_buffer[_autogen_max_size];                     \
                                                                                    \
  *(_autogen_ptr_end--) = '\n';                                                     \
  intptr _autogen_size = CONCAT(sprint_, t1)(v1, _autogen_ptr_end - _autogen_size); \
  String _autogen_msg = {_autogen_ptr_end - _autogen_size, _autogen_size};          \
  print_String(_autogen_msg);                                                       \
})
