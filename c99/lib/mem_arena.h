#pragma once
#include "definitions.h"
#include "mem.h"

// arena
typedef struct {
  Lock lock;
  intptr next;
  intptr end;
} ArenaAllocator;
ArenaAllocator* arena_allocator(Bytes buffer) {
  ArenaAllocator* arena = (ArenaAllocator*)buffer.ptr;
  arena->next = (intptr)(buffer.ptr + sizeof(ArenaAllocator));
  arena->end = (intptr)(buffer.ptr + buffer.size);
  return arena;
}

intptr arena_alloc_impl(ArenaAllocator* arena, Size size, intptr align_mask) {
  get_lock(&arena->lock);
  intptr ptr = (arena->next + align_mask) & ~align_mask;
  intptr next = ptr + (intptr)size;
  intptr end = arena->end;
  arena->next = next;
  release_lock(&arena->lock);

  assert(next <= end);
  zero((byte*)ptr, size);
  return ptr;
}
#define arena_alloc(arena, t) ((t*)arena_alloc_impl(arena, sizeof(t), alignof(t) - 1))
#define arena_alloc2(arena, t, count) ({                          \
  ASSERT_MUlTIPLE_OF(sizeof(t), alignof(t));                      \
  (t*)arena_alloc_impl(arena, sizeof(t) * count, alignof(t) - 1); \
})

void arena_reset(ArenaAllocator* arena, intptr next) {
  get_lock_assert_single_threaded(&arena->lock);
  assert(next >= (intptr)arena && next <= arena->next);
  arena->next = next;
  release_lock(&arena->lock);
}
