#pragma once
#include "definitions.h"

typedef struct {
  intptr start, used, capacity;
} StackAllocator;
#define STACK_ALLOCATOR()                                                      \
  ({                                                                           \
    StackAllocator stack;                                                      \
    intptr start;                                                              \
    READ_STACK_POINTER(start);                                                 \
    stack.start = start;                                                       \
    stack.used = 0;                                                            \
    stack.capacity = 0;                                                        \
    stack;                                                                     \
  })
#define STACK_FREE_ALL(stack) stack.used = 0;
/* If not enough capacity, make more, and return ptr
  NOTE: this can allocate backwards or forwards depending on architecture!
  NOTE: the ABI requires us to align the stack pointer to 16B */
#if ARCH_STACK_GROWS_NEGATIVE
#define STACK_ALLOC(stack, size)                                               \
  ({                                                                           \
    stack.used += size;                                                        \
    intptr ptr = stack.start - stack.used;                                     \
                                                                               \
    intptr diff = stack.used - stack.capacity;                                 \
    if (diff > 0) {                                                            \
      diff = (diff + 15) & ~15;                                                \
      stack.capacity += diff;                                                  \
      STACK_RESERVE(diff);                                                     \
    }                                                                          \
    (byte *)ptr;                                                               \
  })
#else
#define STACK_ALLOC(stack, size)                                               \
  ({                                                                           \
    intptr ptr = stack.start + stack.used;                                     \
    stack.used += size;                                                        \
                                                                               \
    intptr diff = stack.used - stack.capacity;                                 \
    if (diff > 0) {                                                            \
      diff = (diff + 15) & ~15;                                                \
      stack.capacity += diff;                                                  \
      STACK_RESERVE(diff);                                                     \
    }                                                                          \
    (byte *)ptr;                                                               \
  })
#endif
