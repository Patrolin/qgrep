#pragma once
#include "definitions.h"

#if ARCH_X64
#define READ_STACK_POINTER(register_or_memory) \
  __asm__ volatile("movq %%rsp, %0" : "=g"(register_or_memory))
#define STACK_RESERVE(size) \
  __asm__ volatile("subq %0, %%rsp" : : "g"(size) : "rsp")
#define STACK_FREE(size) \
  __asm__ volatile("addq %0, %%rsp" : : "g"(size) : "rsp")
#else
#define READ_STACK_POINTER(variable) ASSERT(false);
#define STACK_RESERVE(variable) ASSERT(false);
#define STACK_FREE(variable) ASSERT(false);
#endif

typedef struct {
  intptr start, used, capacity;
} StackAllocator;
#define stack_allocator() ({     \
  intptr start;                  \
  READ_STACK_POINTER(start);     \
  (StackAllocator){start, 0, 0}; \
})
#define stack_free_all(stack) ({stack.used = 0})
#define stack_destroy(stack) STACK_FREE(stack.capacity)
/* If not enough capacity, make more, and return `ptr_end`
  NOTE: this can allocate backwards or forwards depending on architecture!
  NOTE: the ABI requires us to align the stack pointer to 16B in some cases, so we just do it always */
#if ARCH_STACK_DIRECTION == -1
#define STACK_ALLOC(stack, size) ({            \
  intptr ptr_end = stack.start - stack.used;   \
  stack.used += size;                          \
  /* intptr ptr = stack.start - stack.used; */ \
                                               \
  intptr diff = stack.used - stack.capacity;   \
  if (diff > 0) {                              \
    diff = (diff + 15) & ~15;                  \
    stack.capacity += diff;                    \
    STACK_RESERVE(diff);                       \
  }                                            \
  (byte *)ptr_end;                             \
})
#else
#define STACK_ALLOC(stack, size) ({            \
  /* intptr ptr = stack.start + stack.used; */ \
  stack.used += size;                          \
  intptr ptr_end = stack.start + stack.used;   \
                                               \
  intptr diff = stack.used - stack.capacity;   \
  if (diff > 0) {                              \
    diff = (diff + 15) & ~15;                  \
    stack.capacity += diff;                    \
    STACK_RESERVE(diff);                       \
  }                                            \
  (byte *)ptr_end;                             \
})
#endif
