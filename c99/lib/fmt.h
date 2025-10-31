#pragma once
#include "definitions.h"
#include "os.h"
#include "process.h" /* IWYU pragma: keep */
// #define ASSERT(condition) condition && (fprintf(stderr, "%:% %", __FILE__, __LINE__, #expr), abort());

// sprint()
#define sprint_size_String(value) (value.size)
intptr sprint_String(String str, byte *buffer_end) {
  byte *buffer = buffer_end - str.size;
  for (intptr i = 0; i < str.size; i++) {
    buffer[i] = str.ptr[i];
  }
  return str.size;
}

/* NOTE: log10_ceil(max(type)) */
#define sprint_size_u64(value) (20)
#define sprint_size_u32(value) (10)
#define sprint_size_u16(value) (5)
#define sprint_size_u8(value) (3)
intptr sprint_u64(u64 value, byte *buffer_end) {
  intptr i = 0;
  do {
    intptr digit = value % 10;
    value = value / 10;
    buffer_end[--i] = '0' + (byte)digit;
  } while (value != 0);
  return -i;
}
intptr sprint_u32(u32 value, byte *buffer_end) {
  return sprint_u64(value, buffer_end);
}
intptr sprint_u16(u16 value, byte *buffer_end) {
  return sprint_u64(value, buffer_end);
}
intptr sprint_u8(u8 value, byte *buffer_end) {
  return sprint_u64(value, buffer_end);
}

#if ARCH_IS_64BIT
#define sprint_size_uintptr(value) sprint_size_u64(value)
intptr sprint_uintptr(uintptr value, byte *buffer_end) {
  return sprint_u64(value, buffer_end);
}
#elif ARCH_IS_32BIT
#define sprint_size_uintptr(value) sprint_size_u32(value)
intptr sprint_uintptr(uintptr value, byte *buffer_end) {
  return sprint_u32(value, buffer_end);
}
#endif

/* NOTE: log10_ceil(max(type)) + 1 */
#define sprint_size_i64(value) (19 + 1)
#define sprint_size_i32(value) (10 + 1)
#define sprint_size_i16(value) (5 + 1)
#define sprint_size_i8(value) (3 + 1)
intptr sprint_i64(i64 value, byte *buffer_end) {
  uintptr value_uintptr = (uintptr)((value << 1) >> 1);
  intptr size = sprint_uintptr(value_uintptr, buffer_end);
  if (value < 0) {
    *(buffer_end - size - 1) = '-';
    size += 1;
  }
  return size;
}
intptr sprint_i32(i32 value, byte *buffer_end) {
  return sprint_i64(value, buffer_end);
}
intptr sprint_i16(i16 value, byte *buffer_end) {
  return sprint_i64(value, buffer_end);
}
intptr sprint_i8(i8 value, byte *buffer_end) {
  return sprint_i64(value, buffer_end);
}

#if ARCH_IS_64BIT
#define sprint_size_intptr(value) sprint_size_i64(value)
intptr sprint_intptr(intptr value, byte *buffer_end) {
  return sprint_i64(value, buffer_end);
}
#elif ARCH_IS_32BIT
#define sprint_size_intptr(value) sprint_size_i32(value)
intptr sprint_intptr(uintptr value, byte *buffer_end) {
  return sprint_i32(value, buffer_end);
}
#endif

// stack_print()
#define stack_print(t1, v1) ({                                      \
  intptr _autogen_max_size = CONCAT(sprint_size_, t1)(v1);          \
  byte _autogen_buffer[_autogen_max_size];                          \
  byte *_autogen_ptr_end = &buffer[_autogen_max_size];              \
                                                                    \
  intptr _autogen_size = CONCAT(sprint_, t1)(v1, _autogen_ptr_end); \
  (String){_autogen_ptr_end - _autogen_size + 1, _autogen_size};    \
})
#define stack_println(t1, v1) ({                                    \
  intptr _autogen_max_size = CONCAT(sprint_size_, t1)(v1) + 1;      \
  byte _autogen_buffer[_autogen_max_size];                          \
  byte *_autogen_ptr_end = &_autogen_buffer[_autogen_max_size];     \
                                                                    \
  *(--_autogen_ptr_end) = '\n';                                     \
  intptr _autogen_size = CONCAT(sprint_, t1)(v1, _autogen_ptr_end); \
  (String){_autogen_ptr_end - _autogen_size, _autogen_size + 1};    \
})

// print()
void print_String(String str) {
#if OS_WINDOWS
  DWORD chars_written;
  WriteConsoleA(stdout, str.ptr, (DWORD)str.size, &chars_written, 0);
  // TODO: assert(chars_written == str.size)
#else
  todo.assert;
#endif
}
#define print_copy(t1, v1) ({                \
  String _autogen_msg = stack_print(t1, v1); \
  print_String(_autogen_msg);                \
})
#define print(t1, v1) IF(IS_STRING(t1), print_String(v1), print_copy(t1, v1))
#define println(t1, v1) ({                     \
  String _autogen_msg = stack_println(t1, v1); \
  print_String(_autogen_msg);                  \
})
