#pragma once
#include "definitions.h"

/* NOTE: true on almost all architectures */
#if ARCH_X64
#define READ_STACK_POINTER(register_or_memory) \
  __asm__ volatile("movq %%rsp, %0" : "=g"(register_or_memory));
#define STACK_RESERVE(size) \
  __asm__ volatile("subq %0, %%rsp" : : "g"(size) : "rsp");
#else
#define READ_STACK_POINTER(variable) ASSERT(false);
#endif

typedef struct {
  intptr start, used, capacity;
} StackAllocator;
#define stack_allocator() ({     \
  intptr start;                  \
  READ_STACK_POINTER(start);     \
  (StackAllocator){start, 0, 0}; \
})
#define stack_free_all(stack) stack.used = 0;
/* If not enough capacity, make more, and return ptr
  NOTE: this can allocate backwards or forwards depending on architecture!
  NOTE: the ABI requires us to align the stack pointer to 16B */
#if ARCH_STACK_GROWS_NEGATIVE
#define STACK_ALLOC(stack, size) ({          \
  stack.used += size;                        \
  intptr ptr = stack.start - stack.used;     \
                                             \
  intptr diff = stack.used - stack.capacity; \
  if (diff > 0) {                            \
    diff = (diff + 15) & ~15;                \
    stack.capacity += diff;                  \
    STACK_RESERVE(diff);                     \
  }                                          \
  (byte *)ptr;                               \
})
#else
#define STACK_ALLOC(stack, size) ({          \
  intptr ptr = stack.start + stack.used;     \
  stack.used += size;                        \
                                             \
  intptr diff = stack.used - stack.capacity; \
  if (diff > 0) {                            \
    diff = (diff + 15) & ~15;                \
    stack.capacity += diff;                  \
    STACK_RESERVE(diff);                     \
  }                                          \
  (byte *)ptr;                               \
})
#endif
