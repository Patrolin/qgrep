#pragma once
#include "definitions.h"
#include "os.h"
#include "process.h"
// #define ASSERT(condition) condition && (fprintf(stderr, "%:% %", __FILE__, __LINE__, #expr), abort());

// stack_print()
#define stack_print(stack, t1, v1) ({                     \
  intptr max_size = CONCAT(sprint_size_, t1)(v1);         \
  byte *ptr_end = (byte *)(STACK_ALLOC(stack, max_size)); \
                                                          \
  intptr size = CONCAT(sprint_, t1)(v1, ptr_end);         \
                                                          \
  (String){ptr_end - size + 1, size};                     \
})
#define stack_println(stack, t1, v1) ({                   \
  intptr max_size = CONCAT(sprint_size_, t1)(v1) + 1;     \
  byte *ptr_end = (byte *)(STACK_ALLOC(stack, max_size)); \
  *(ptr_end--) = '\n';                                    \
                                                          \
  intptr size = CONCAT(sprint_, t1)(v1, ptr_end);         \
                                                          \
  (String){ptr_end - size + 1, size + 1};                 \
})

// sprint()
#define sprint_size_String(value) (value.size)
intptr sprint_String(String str, byte *buffer_end) {
  byte *buffer = buffer_end - str.size + 1;
  for (intptr i = 0; i < str.size; i++) {
    buffer[i] = str.ptr[i];
  }
  return str.size;
}

#define sprint_size_uintptr(value) (20)
intptr sprint_uintptr(uintptr value, byte *buffer_end) {
  intptr i = 0;
  do {
    intptr digit = value % 10;
    value = value / 10;
    buffer_end[i--] = '0' + digit;
  } while (value != 0);
  return -i;
}

#define sprint_size_intptr(value) (sprint_size_uintptr(value) + 1)
intptr sprint_intptr(intptr value, byte *buffer_end) {
  intptr size = sprint_uintptr((value << 1) >> 1, buffer_end);
  if (value < 0) {
    *(buffer_end - size) = '-';
    size += 1;
  }
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
