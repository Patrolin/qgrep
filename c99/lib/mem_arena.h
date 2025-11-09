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
  return ptr;
}
#define arena_alloc(arena, t) ((t*)arena_alloc_impl(arena, sizeof(t)))
#define arena_alloc2(arena, t, count) ((t*)arena_alloc_impl(arena, sizeof(t) * count))
void arena_free(ArenaAllocator* arena, intptr ptr) {
  assert(ptr >= (intptr)arena && ptr <= (intptr)arena->next);
  arena->next = ptr;
}

// sub arena
ArenaAllocator* sub_arena(ArenaAllocator* arena) {
  ArenaAllocator* subArena = arena_alloc(arena, ArenaAllocator);
  *subArena = *arena;
  return subArena;
}
