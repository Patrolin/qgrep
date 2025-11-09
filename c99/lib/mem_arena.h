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

intptr arena_alloc_impl(ArenaAllocator* arena, Size size) {
  get_lock(&arena->lock);
  intptr ptr = (intptr)arena->next;
  arena->next += size;
  release_lock(&arena->lock);
  zero((byte*)ptr, size);
  return ptr;
}
#define arena_alloc(arena, t) ((t*)arena_alloc_impl(arena, sizeof(t)))
#define arena_alloc2(arena, t, count) ((t*)arena_alloc_impl(arena, sizeof(t) * count))

void arena_reset(ArenaAllocator* arena, intptr next) {
  get_lock_assert_single_threaded(&arena->lock);
  assert(next >= (intptr)arena && next <= (intptr)arena->next);
  arena->next = next;
  release_lock(&arena->lock);
}
