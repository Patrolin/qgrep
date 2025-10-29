#pragma once
#include "definitions.h"
#include "os.h"
#include "process.h"
// #define ASSERT(condition) condition && (fprintf(stderr, "%:% %", __FILE__, __LINE__, #expr), abort());

// stack_print()
#define stack_print(stack, t1, v1) ({                 \
  intptr max_size = CONCAT(sprint_size_, t1)(v1);     \
  byte *ptr = (byte *)(STACK_ALLOC(stack, max_size)); \
                                                      \
  intptr size = CONCAT(sprint_, t1)(v1, ptr);         \
                                                      \
  (String){ptr + max_size - size, size};              \
})
#if ARCH_STACK_GROWS_NEGATIVE
#define stack_println(stack, t1, v1) ({                 \
  String s2 = stack_print(stack, String, string("\n")); \
  String s1 = stack_print(stack, t1, v1);               \
  (String){s1.ptr, s1.size + s2.size};                  \
})
#else
#define stack_println(stack, t1, v1) ({                 \
  String s1 = stack_print(stack, t1, v1);               \
  String s2 = stack_print(stack, String, string("\n")); \
  (String){s1.ptr, s1.size + s2.size};                  \
})
#endif

// sprint()
#define sprint_size_String(value) (value.size)
intptr sprint_String(String str, byte *buffer) {
  intptr i = 0;
  for (; i < str.size; i++) {
    buffer[i] = str.ptr[i];
  }
  return i;
}

#define sprint_size_uintptr(value) (20)
intptr sprint_uintptr(uintptr value, byte *buffer) {
  intptr size = sprint_size_uintptr(value);
  intptr i = size;
  do {
    intptr digit = value % 10;
    value = value / 10;
    buffer[i--] = '0' + digit;
  } while (i >= 0 && value != 0);
  return size - i;
}

#define sprint_size_intptr(value) (sprint_size_uintptr(value) + 1)
intptr sprint_intptr(intptr value, byte *buffer) {
  intptr size = 0;
  if (value < 0) {
    *buffer = '-';
    size += 1;
  }
  size += sprint_uintptr((value << 1) >> 1, buffer + size);
  return size;
}

// print()
void print_String(String str) {
#if OS_WINDOWS
  DWORD chars_written;
  WriteConsoleA(stdout, str.ptr, str.size, &chars_written, 0);
  // TODO: assert(chars_written == str.size)
#else
  todo.assert;
#endif
}
void print_uintptr(uintptr value) {
  print_String(string("print_uintptr()\n"));
};

#define print_copy(t1, v1) ({               \
  StackAllocator stack = stack_allocator(); \
  String msg = stack_print(stack, t1, v1);  \
  print_String(msg);                        \
})
#define print(t1, v1) IF(IS_STRING(t1), print_String(v1), print_copy(t1, v1))
#define println(t1, v1) ({                   \
  StackAllocator stack = stack_allocator();  \
  String msg = stack_println(stack, t1, v1); \
  print_String(msg);                         \
})
