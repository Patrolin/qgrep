// #define ASSERT(condition) condition && (fprintf(stderr, "%:% %", __FILE__, __LINE__, #expr), abort());

typedef struct {
  intptr start, used, capacity;
} StackAllocator;
#define STACK_ALLOCATOR() ({ \
  StackAllocator stack;      \
  intptr start;              \
  READ_STACK_POINTER(start); \
  stack.start = start;       \
  stack.used = 0;            \
  stack.capacity = 0;        \
  stack;                     \
})
/* If not enough capacity, make more, and return ptr
  NOTE: this can allocate backwards or forwards depending on architecture!
  NOTE: the ABI requires us to align the stack pointer to 16B */
#if ARCH_STACK_GROWS_NEGATIVE
#define STACK_ALLOC(stack, size) ({       \
  stack.used += size;                     \
  int diff = stack.used - stack.capacity; \
  if (diff > 0) {                         \
    diff = (diff + 15) & ~15;             \
    stack.capacity += diff;               \
    STACK_RESERVE(diff);                  \
  }                                       \
  (byte*)(stack.start - stack.used);      \
})
#else
#define STACK_ALLOC(stack, size) ({       \
  intptr ptr = stack.start + stack.used;  \
  stack.used += size;                     \
  int diff = stack.used - stack.capacity; \
  if (diff > 0) {                         \
    diff = (diff + 15) & ~15;             \
    stack.capacity += diff;               \
    STACK_RESERVE(diff);                  \
  }                                       \
  (byte*)ptr;                             \
})
#endif
#define STACK_FREE_ALL(stack) \
  stack.start = 0;            \
  stack.used = 0;

#define SPRINT_SIZE_String(value) value.size
#define SPRINT_SIZE_uintptr(value) 20

#define STACK_PRINT(stack, t1, v1) ({                \
  intptr max_size = CONCAT(SPRINT_SIZE_, t1)(v1);    \
  byte* ptr = (byte*)(STACK_ALLOC(stack, max_size)); \
                                                     \
  intptr size = CONCAT(sprint_, t1)(v1, ptr);        \
  stack.used += size;                                \
                                                     \
  (String){ptr, size};                               \
})
#define STACK_PRINT2(stack, t1, v1, t2, v2) ({       \
  intptr max_size = CONCAT(SPRINT_SIZE_, t1)(v1);    \
  max_size += CONCAT(SPRINT_SIZE_, t2)(v2);          \
  byte* ptr = (byte*)(STACK_ALLOC(stack, max_size)); \
                                                     \
  intptr size = CONCAT(sprint_, t1)(v1, ptr);        \
  size += CONCAT(sprint_, t2)(v2, ptr + size);       \
  stack.used += size;                                \
                                                     \
  (String){ptr, size};                               \
})
#define STACK_PRINTLN(stack, t1, v1) STACK_PRINT(stack, t1, v1, String, String("\n"));

intptr sprint_String(String str, byte* buffer) {
  intptr i = 0;
  for (; i < str.size; i++) {
    buffer[i] = str.ptr[i];
  }
  return i;
}
intptr sprint_uintptr(uintptr value, byte* buffer) {
  intptr size = SPRINT_SIZE_uintptr(value) - 1;
  intptr i = size;
  buffer[i--] = 0;
  do {
    intptr digit = value % 10;
    value = value / 10;
    buffer[i--] = '0' + digit;
  } while (i >= 0 && value != 0);
  return size - i;
}

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
  print_String(STRING("print_uintptr()\n"));
};

#define PRINT(t1, v1) CONCAT(print_, t1)(v1);
