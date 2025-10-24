
typedef struct {
  intptr start, used, capacity;
} StackAllocator;
#define STACK_ALLOCATOR() ((StackAllocator){})
/* NOTE: this can allocate backwards or forwards depending on architecture!
  TODO: if not enough capacity, make more, and return ptr */
#define STACK_ALLOC(stack, size) TODO;
#define STACK_FREE_ALL(stack) \
  stack.start = 0;            \
  stack.used = 0;

#define SPRINT_SIZE_String(value) value.size
#define SPRINT_SIZE_uintptr(value) 20

#define SPRINT(stack, t1, v1) ({                                      \
  intptr max_size = CONCAT(SPRINT_SIZE_, t1)(v1);                     \
  byte* buffer = (byte*)(STACK_ALLOC(stack, max_size));               \
  intptr size = CONCAT(sprint_, t1)(v1, ptr);                         \
  stack.used += size;                                                 \
  byte* ptr = (byte*)(&stack) + sizeof(StackAllocator) + stack.start; \
  intptr size = stack.current - stack.start;                          \
  start = stack.current;                                              \
  String str = (String){ptr, size};                                   \
});
#define SPRINTLN(stack, t1, v1) ({     \
  SPRINT(stack, t1, v1);               \
  SPRINT(stack, String, STRING("\n")); \
})

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
  DWORD bytes_written;
  WriteConsoleA(stdout, str.ptr, str.size, &bytes_written, 0);
}
void print_uintptr(uintptr value) {
  print_String(STRING("print_uintptr()\n"));
};

#define PRINT(t1, v1) CONCAT(print_, t1)(v1);
