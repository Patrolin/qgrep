#pragma once
#include "definitions.h"
#include "math.h"
#include "os.h"

// sprint()
#define sprint_size1(t1, v1) (CONCAT(sprint_size_, t1)(v1))
#define sprint_size2(t1, v1, t2, v2) (CONCAT(sprint_size_, t1)(v1) + CONCAT(sprint_size_, t2)(v2))
#define sprint_size3(t1, v1, t2, v2, t3, v3) (CONCAT(sprint_size_, t1)(v1) + CONCAT(sprint_size_, t2)(v2) + CONCAT(sprint_size_, t3)(v3))
#define sprint_size4(t1, v1, t2, v2, t3, v3, t4, v4) (CONCAT(sprint_size_, t1)(v1) + CONCAT(sprint_size_, t2)(v2) + CONCAT(sprint_size_, t3)(v3) + CONCAT(sprint_size_, t4)(v4))
#define STACK_BUFFER(buffer, max_size, ptr_end) \
  byte buffer[max_size];                        \
  byte* ptr_end = &buffer[max_size]
#define sprint(t1, v1, ptr_end) CONCAT0(sprint_, t1)(v1, ptr_end)
#define sprint_to_string(ptr_end, size) ((String){ptr_end - (intptr)size, size})

#define sprint_size_String(value) (value.size)
Size sprint_String(String str, byte* buffer_end) {
  byte* buffer = buffer_end - str.size;
  for (intptr i = 0; i < str.size; i++) {
    buffer[i] = str.ptr[i];
  }
  return str.size;
}

#define sprint_size__Bool(value) 5
Size sprint__Bool(bool value, byte* buffer_end) {
  String msg = value ? string("true") : string("false");
  return sprint_String(msg, buffer_end);
}

/* NOTE: log10_ceil(max(type)) == log10_ceil(pow(2, bits) - 1) */
#define sprint_size_u64(value) (20)
#define sprint_size_u32(value) (10)
#define sprint_size_u16(value) (5)
#define sprint_size_u8(value) (3)
#define sprint_size_byte(value) (3)
Size sprint_u64(u64 value, byte* buffer_end) {
  intptr i = 0;
  do {
    intptr digit = value % 10;
    value = value / 10;
    buffer_end[--i] = '0' + (byte)digit;
  } while (value != 0);
  return (Size)(-i);
}
Size sprint_u32(u32 value, byte* buffer_end) {
  return sprint_u64(value, buffer_end);
}
Size sprint_u16(u16 value, byte* buffer_end) {
  return sprint_u64(value, buffer_end);
}
Size sprint_u8(u8 value, byte* buffer_end) {
  return sprint_u64(value, buffer_end);
}
Size sprint_byte(u8 value, byte* buffer_end) {
  return sprint_u64(value, buffer_end);
}

/* NOTE: log10_ceil(-min(type)) + 1 == log10_ceil(pow(2, bits-1)) + 1 */
#define sprint_size_i64(value) (20)
#define sprint_size_i32(value) (11)
#define sprint_size_i16(value) (6)
#define sprint_size_i8(value) (4)
Size sprint_i64(i64 value, byte* buffer_end) {
  u64 value_abs = (u64)abs(value);
  Size size = sprint_u64(value_abs, buffer_end);
  if (value < 0) {
    *(buffer_end - size - 1) = '-';
    size += 1;
  }
  return size;
}
Size sprint_i32(i32 value, byte* buffer_end) {
  u64 value_abs = (u64)abs(value);
  Size size = sprint_u64(value_abs, buffer_end);
  if (value < 0) {
    *(buffer_end - size - 1) = '-';
    size += 1;
  }
  return size;
}
Size sprint_i16(i16 value, byte* buffer_end) {
  u64 value_abs = (u64)abs(value);
  Size size = sprint_u64(value_abs, buffer_end);
  if (value < 0) {
    *(buffer_end - size - 1) = '-';
    size += 1;
  }
  return size;
}
Size sprint_i8(i8 value, byte* buffer_end) {
  u64 value_abs = (u64)abs(value);
  Size size = sprint_u64(value_abs, buffer_end);
  if (value < 0) {
    *(buffer_end - size - 1) = '-';
    size += 1;
  }
  return size;
}

#define sprint_size_uhex_pad(value) (2 + 2 * sizeof(u64))
byte* HEX_DIGITS = "0123456789ABCDEF";
Size sprint_uhex_pad(u64 value, byte* buffer_end) {
  intptr i = 0;
  do {
    u64 digit = value & 0xf;
    value = value >> 4;
    buffer_end[--i] = HEX_DIGITS[digit];
  } while (i > -2 * (intptr)sizeof(u64));
  buffer_end[--i] = 'x';
  buffer_end[--i] = '0';
  return (Size)(-i);
}
#define sprint_size_uhex(value) (2 + 2 * sizeof(u64))
Size sprint_uhex(u64 value, byte* buffer_end) {
  intptr i = 0;
  do {
    u64 digit = value & 0xf;
    value = value >> 4;
    buffer_end[--i] = HEX_DIGITS[digit];
  } while (value != 0);
  buffer_end[--i] = 'x';
  buffer_end[--i] = '0';
  return (Size)(-i);
}
#define sprint_size_ihex(value) sprint_size_uhex(value)
#define sprint_ihex(value, buffer_end) sprint_uhex((u64)(value), buffer_end)
#define sprint_size_fhex(value) sprint_size_uhex(value)
#define sprint_fhex(value, buffer_end) sprint_fhex_impl(__COUNTER__, value, buffer_end)
#define sprint_fhex_impl(c, value, buffer_end) sprint_uhex(reinterpret(value, f64, u64), buffer_end)

#if ARCH_IS_64_BIT
  #define sprint_size_uintptr(value) sprint_size_u64(value)
Size sprint_uintptr(uintptr value, byte* buffer_end) {
  return sprint_u64((u64)value, buffer_end);
}
  #define sprint_size_intptr(value) sprint_size_u64(value)
Size sprint_intptr(intptr value, byte* buffer_end) {
  return sprint_u64((u64)value, buffer_end);
}
#elif ARCH_IS_32_BIT
  #define sprint_size_uintptr(value) sprint_size_u32(value)
Size sprint_uintptr(uintptr value, byte* buffer_end) {
  return sprint_u32((u32)value, buffer_end);
}
  #define sprint_size_intptr(value) sprint_size_u32(value)
Size sprint_intptr(intptr value, byte* buffer_end) {
  return sprint_u32((u32)value, buffer_end);
}
#endif

// sprintf()
#define sprintf1(ptr_end, format, t1, v1) sprintf1_impl(__COUNTER__, ptr_end, format, t1, v1)
#define sprintf1_impl(c, ptr_end, format, t1, v1) ({                           \
  String VAR(fmt, c) = format;                                                 \
  Size VAR(size, c) = 0;                                                       \
  intptr VAR(i, c) = (intptr)VAR(fmt, c).size;                                 \
  intptr VAR(j, c) = VAR(i, c);                                                \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint(String, VAR(after, c), ptr_end);                  \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += sprint(t1, v1, ptr_end - VAR(size, c));                  \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  if (VAR(j, c) > 0) {                                                         \
    String VAR(before, c) = str_slice(VAR(fmt, c), 0, VAR(j, c));              \
    VAR(size, c) += sprint(String, VAR(before, c), ptr_end - VAR(size, c));    \
  }                                                                            \
  VAR(size, c);                                                                \
})
#define sprintf2(ptr_end, format, t1, v1, t2, v2) sprintf2_impl(__COUNTER__, ptr_end, format, t1, v1, t2, v2)
#define sprintf2_impl(c, ptr_end, format, t1, v1, t2, v2) ({                   \
  String VAR(fmt, c) = format;                                                 \
  Size VAR(size, c) = 0;                                                       \
  intptr VAR(i, c) = (intptr)VAR(fmt, c).size;                                 \
  intptr VAR(j, c) = VAR(i, c);                                                \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint(String, VAR(after, c), ptr_end);                  \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += sprint(t2, v2, ptr_end - VAR(size, c));                  \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint(String, VAR(after, c), ptr_end - VAR(size, c));   \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += sprint(t1, v1, ptr_end - VAR(size, c));                  \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  if (VAR(j, c) > 0) {                                                         \
    String VAR(before, c) = str_slice(VAR(fmt, c), 0, VAR(j, c));              \
    VAR(size, c) += sprint(String, VAR(before, c), ptr_end - VAR(size, c));    \
  }                                                                            \
  VAR(size, c);                                                                \
})
#define sprintf3(ptr_end, format, t1, v1, t2, v2, t3, v3) sprintf3_impl(__COUNTER__, ptr_end, format, t1, v1, t2, v2, t3, v3)
#define sprintf3_impl(c, ptr_end, format, t1, v1, t2, v2, t3, v3) ({           \
  String VAR(fmt, c) = format;                                                 \
  Size VAR(size, c) = 0;                                                       \
  intptr VAR(i, c) = (intptr)VAR(fmt, c).size;                                 \
  intptr VAR(j, c) = VAR(i, c);                                                \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint(String, VAR(after, c), ptr_end);                  \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += sprint(t3, v3, ptr_end - VAR(size, c));                  \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint(String, VAR(after, c), ptr_end - VAR(size, c));   \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += sprint(t2, v2, ptr_end - VAR(size, c));                  \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  while (VAR(i, c) > 0) {                                                      \
    (VAR(i, c)--);                                                             \
    if (VAR(fmt, c).ptr[VAR(i, c)] == '%') {                                   \
      String VAR(after, c) = str_slice(VAR(fmt, c), VAR(i, c) + 1, VAR(j, c)); \
      VAR(size, c) += sprint(String, VAR(after, c), ptr_end - VAR(size, c));   \
      VAR(j, c) = VAR(i, c);                                                   \
      VAR(size, c) += sprint(t1, v1, ptr_end - VAR(size, c));                  \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  if (VAR(j, c) > 0) {                                                         \
    String VAR(before, c) = str_slice(VAR(fmt, c), 0, VAR(j, c));              \
    VAR(size, c) += sprint(String, VAR(before, c), ptr_end - VAR(size, c));    \
  }                                                                            \
  VAR(size, c);                                                                \
})

// fprint()
void fprint(FileHandle file, String str) {
#if OS_WINDOWS
  DWORD bytes_written;
  DWORD chars_to_write = downcast(Size, str.size, DWORD);
  WriteFile(file, str.ptr, chars_to_write, &bytes_written, 0);
  assert(bytes_written == str.size);
#elif OS_LINUX
  intptr bytes_written = write(file, str.ptr, str.size);
  assert(bytes_written == str.size);
#else
  assert(false);
#endif
}

// assert()
#undef assert
#define assert(condition) assert_impl(__COUNTER__, condition, __FILE__ ":" STR(__LINE__) " assert(" #condition ")\n")
#define assert1(condition, msg_cstr) assert_impl(__COUNTER__, condition, __FILE__ ":" STR(__LINE__) " " msg_cstr "\n")
#define assert_impl(c, condition, msg_cstr) ({ \
  if (!(condition)) {                          \
    fprint(STDERR, string(msg_cstr));          \
    abort();                                   \
  }                                            \
})

// print()
void print_String(String str) {
  fprint(STDOUT, str);
}
#define print_copy(t1, v1) print_copy_impl(__COUNTER__, t1, v1)
#define print_copy_impl(c, t1, v1) ({                                   \
  intptr VAR(max_size, c) = sprint_size1(t1, v1);                       \
  STACK_BUFFER(VAR(buffer, c), VAR(max_size, c), VAR(ptr_end, c));      \
                                                                        \
  Size VAR(size, c) = CONCAT(sprint_, t1)(v1, VAR(ptr_end, c));         \
  String VAR(msg, c) = sprint_to_string(VAR(ptr_end, c), VAR(size, c)); \
  print_String(VAR(msg, c));                                            \
})
#define print(t1, v1) IF(IS_STRING(t1), print_String(v1), print_copy(t1, v1))
#define println(t1, v1) println_impl(__COUNTER__, t1, v1)
#define println_impl(c, t1, v1) ({                                      \
  intptr VAR(max_size, c) = sprint_size1(t1, v1) + 1;                   \
  STACK_BUFFER(VAR(buffer, c), VAR(max_size, c), VAR(ptr_end, c));      \
                                                                        \
  *(VAR(ptr_end, c) - 1) = '\n';                                        \
  Size VAR(size, c) = CONCAT(sprint_, t1)(v1, VAR(ptr_end, c) - 1) + 1; \
  String VAR(msg, c) = sprint_to_string(VAR(ptr_end, c), VAR(size, c)); \
  print_String(VAR(msg, c));                                            \
})

// printf()
#define printf1(format, t1, v1) printf1_impl(__COUNTER__, format, t1, v1)
#define printf1_impl(c, format, t1, v1) ({                              \
  intptr VAR(max_size, c) = sprint_size2(String, format, t1, v1);       \
  STACK_BUFFER(VAR(buffer, c), VAR(max_size, c), VAR(ptr_end, c));      \
                                                                        \
  Size VAR(size, c) = sprintf1(VAR(ptr_end, c), format, t1, v1);        \
  String VAR(msg, c) = sprint_to_string(VAR(ptr_end, c), VAR(size, c)); \
  print_String(VAR(msg, c));                                            \
})
#define printf2(format, t1, v1, t2, v2) printf2_impl(__COUNTER__, format, t1, v1, t2, v2)
#define printf2_impl(c, format, t1, v1, t2, v2) ({                        \
  intptr VAR(max_size, c) = sprint_size3(String, format, t1, v1, t2, v2); \
  STACK_BUFFER(VAR(buffer, c), VAR(max_size, c), VAR(ptr_end, c));        \
                                                                          \
  Size VAR(size, c) = sprintf2(VAR(ptr_end, c), format, t1, v1, t2, v2);  \
  String VAR(msg, c) = sprint_to_string(VAR(ptr_end, c), VAR(size, c));   \
  print_String(VAR(msg, c));                                              \
})
#define printf3(format, t1, v1, t2, v2, t3, v3) printf3_impl(__COUNTER__, format, t1, v1, t2, v2, t3, v3)
#define printf3_impl(c, format, t1, v1, t2, v2, t3, v3) ({                        \
  intptr VAR(max_size, c) = sprint_size4(String, format, t1, v1, t2, v2, t3, v3); \
  STACK_BUFFER(VAR(buffer, c), VAR(max_size, c), VAR(ptr_end, c));                \
                                                                                  \
  Size VAR(size, c) = sprintf3(VAR(ptr_end, c), format, t1, v1, t2, v2, t3, v3);  \
  String VAR(msg, c) = sprint_to_string(VAR(ptr_end, c), VAR(size, c));           \
  print_String(VAR(msg, c));                                                      \
})

// printfln()
#define printfln1(format, t1, v1) printfln1_impl(__COUNTER__, format, t1, v1)
#define printfln1_impl(c, format, t1, v1) ({                             \
  Size VAR(max_size, c) = sprint_size2(String, format, t1, v1) + 1;      \
  STACK_BUFFER(VAR(buffer, c), VAR(max_size, c), VAR(ptr_end, c));       \
                                                                         \
  *(VAR(ptr_end, c) - 1) = '\n';                                         \
  Size VAR(size, c) = sprintf1(VAR(ptr_end, c) - 1, format, t1, v1) + 1; \
  String VAR(msg, c) = sprint_to_string(VAR(ptr_end, c), VAR(size, c));  \
  print_String(VAR(msg, c));                                             \
})
#define printfln2(format, t1, v1, t2, v2) printfln2_impl(__COUNTER__, format, t1, v1, t2, v2)
#define printfln2_impl(c, format, t1, v1, t2, v2) ({                             \
  Size VAR(max_size, c) = sprint_size3(String, format, t1, v1, t2, v2) + 1;      \
  STACK_BUFFER(VAR(buffer, c), VAR(max_size, c), VAR(ptr_end, c));               \
                                                                                 \
  *(VAR(ptr_end, c) - 1) = '\n';                                                 \
  Size VAR(size, c) = sprintf2(VAR(ptr_end, c) - 1, format, t1, v1, t2, v2) + 1; \
  String VAR(msg, c) = sprint_to_string(VAR(ptr_end, c), VAR(size, c));          \
  print_String(VAR(msg, c));                                                     \
})
#define printfln3(format, t1, v1, t2, v2, t3, v3) printfln3_impl(__COUNTER__, format, t1, v1, t2, v2, t3, v3)
#define printfln3_impl(c, format, t1, v1, t2, v2, t3, v3) ({                             \
  Size VAR(max_size, c) = sprint_size4(String, format, t1, v1, t2, v2, t3, v3) + 1;      \
  STACK_BUFFER(VAR(buffer, c), VAR(max_size, c), VAR(ptr_end, c));                       \
                                                                                         \
  *(VAR(ptr_end, c) - 1) = '\n';                                                         \
  Size VAR(size, c) = sprintf3(VAR(ptr_end, c) - 1, format, t1, v1, t2, v2, t3, v3) + 1; \
  String VAR(msg, c) = sprint_to_string(VAR(ptr_end, c), VAR(size, c));                  \
  print_String(VAR(msg, c));                                                             \
})

// IWYU pragma: begin_exports
#include "fmt_float.h"
// IWYU pragma: end_exports
