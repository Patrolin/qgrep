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
#define sprintf1(ptr_end, format, t1, v1) sprintf1_impl(__COUNTER__, ptr_end, format, t1, v1)
#define sprintf1_impl(c, ptr_end, format, t1, v1) ({                           \
  String VAR(fmt, c) = format;                                                 \
  intptr VAR(size, c) = 0;                                                     \
  intptr VAR(i, c) = VAR(fmt, c).size;                                         \
  intptr VAR(j, c) = VAR(i, c);                                                \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint_String(VAR(after, c), ptr_end);                   \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += CONCAT(sprint_, t1)(v1, ptr_end - VAR(size, c));         \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  if (VAR(j, c) > 0) {                                                         \
    String VAR(before, c) = str_slice(VAR(fmt, c), 0, VAR(j, c));              \
    VAR(size, c) += sprint_String(VAR(before, c), ptr_end - VAR(size, c));     \
  }                                                                            \
  VAR(size, c);                                                                \
})
#define sprintf2(ptr_end, format, t1, v1, t2, v2) sprintf2_impl(__COUNTER__, ptr_end, format, t1, v1, t2, v2)
#define sprintf2_impl(c, ptr_end, format, t1, v1, t2, v2) ({                   \
  String VAR(fmt, c) = format;                                                 \
  intptr VAR(size, c) = 0;                                                     \
  intptr VAR(i, c) = VAR(fmt, c).size;                                         \
  intptr VAR(j, c) = VAR(i, c);                                                \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint_String(VAR(after, c), ptr_end);                   \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += CONCAT(sprint_, t2)(v2, ptr_end - VAR(size, c));         \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint_String(VAR(after, c), ptr_end - VAR(size, c));    \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += CONCAT(sprint_, t1)(v1, ptr_end - VAR(size, c));         \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  if (VAR(j, c) > 0) {                                                         \
    String VAR(before, c) = str_slice(VAR(fmt, c), 0, VAR(j, c));              \
    VAR(size, c) += sprint_String(VAR(before, c), ptr_end - VAR(size, c));     \
  }                                                                            \
  VAR(size, c);                                                                \
})
#define sprintf3(ptr_end, format, t1, v1, t2, v2, t3, v3) sprintf3_impl(__COUNTER__, ptr_end, format, t1, v1, t2, v2, t3, v3)
#define sprintf3_impl(c, ptr_end, format, t1, v1, t2, v2, t3, v3) ({           \
  String VAR(fmt, c) = format;                                                 \
  intptr VAR(size, c) = 0;                                                     \
  intptr VAR(i, c) = VAR(fmt, c).size;                                         \
  intptr VAR(j, c) = VAR(i, c);                                                \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint_String(VAR(after, c), ptr_end);                   \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += CONCAT(sprint_, t3)(v3, ptr_end - VAR(size, c));         \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint_String(VAR(after, c), ptr_end - VAR(size, c));    \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += CONCAT(sprint_, t2)(v2, ptr_end - VAR(size, c));         \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint_String(VAR(after, c), ptr_end - VAR(size, c));    \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += CONCAT(sprint_, t1)(v1, ptr_end - VAR(size, c));         \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  if (VAR(j, c) > 0) {                                                         \
    String VAR(before, c) = str_slice(VAR(fmt, c), 0, VAR(j, c));              \
    VAR(size, c) += sprint_String(VAR(before, c), ptr_end - VAR(size, c));     \
  }                                                                            \
  VAR(size, c);                                                                \
})

// assert()
#define assert(condition) assert_impl(__COUNTER__, condition, #condition "\n")
#define assert_impl(c, condition, msg) ({                                                             \
  if (!(condition)) {                                                                                 \
    String VAR(format, c) = string("%:% %");                                                          \
    String VAR(file, c) = string(__FILE__);                                                           \
    intptr VAR(line, c) = __LINE__;                                                                   \
    String VAR(condition_str, c) = string(msg);                                                       \
    intptr VAR(max_size, c) = sprint_size_String(VAR(format, c)) + sprint_size_String(VAR(file, c));  \
    VAR(max_size, c) += sprint_size_intptr(VAR(line, c)) + sprint_size_String(VAR(condition_str, c)); \
                                                                                                      \
    byte VAR(buffer, c)[VAR(max_size, c)];                                                            \
    byte* VAR(ptr_end, c) = &VAR(buffer, c)[VAR(max_size, c)];                                        \
    intptr VAR(size, c) = sprintf3(VAR(ptr_end, c), VAR(format, c), String, VAR(file, c),             \
                                   intptr, VAR(line, c), String, VAR(condition_str, c));              \
    String VAR(error, c) = {VAR(ptr_end, c) - VAR(size, c), VAR(size, c)};                            \
    fprint(STDERR, VAR(error, c));                                                                    \
    abort();                                                                                          \
  }                                                                                                   \
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
#define print_copy(t1, v1) print_copy_impl(__COUNTER__, t1, v1)
#define print_copy_impl(c, t1, v1) ({                                  \
  intptr VAR(max_size, c) = CONCAT(sprint_size_, t1)(v1);              \
  byte VAR(buffer, c)[VAR(max_size, c)];                               \
  byte* VAR(ptr_end, c) = &VAR(buffer, c)[VAR(max_size, c)];           \
                                                                       \
  intptr VAR(size, c) = CONCAT(sprint_, t1)(v1, VAR(ptr_end, c));      \
  String VAR(msg, c) = {VAR(ptr_end, c) - VAR(size, c), VAR(size, c)}; \
  print_String(VAR(msg, c));                                           \
})
#define print(t1, v1) IF(IS_STRING(t1), print_String(v1), print_copy(t1, v1))
#define println(t1, v1) println_impl(__COUNTER__, t1, v1)
#define println_impl(c, t1, v1) ({                                        \
  intptr VAR(max_size, c) = CONCAT(sprint_size_, t1)(v1) + 1;             \
  byte VAR(buffer, c)[VAR(max_size, c)];                                  \
  byte* VAR(ptr_end, c) = &VAR(buffer, c)[VAR(max_size, c)];              \
                                                                          \
  *(VAR(ptr_end, c) - 1) = '\n';                                          \
  intptr VAR(size, c) = CONCAT(sprint_, t1)(v1, VAR(ptr_end, c) - 1) + 1; \
  String VAR(msg, c) = {VAR(ptr_end, c) - VAR(size, c), VAR(size, c)};    \
  print_String(VAR(msg, c));                                              \
})
