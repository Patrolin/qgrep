#pragma once
#include "definitions.h"

typedef struct {
  intptr next;
  intptr end;
} ArenaAllocator;
ArenaAllocator* arena_allocator(Bytes buffer) {
  ArenaAllocator* arena = (ArenaAllocator*)buffer.ptr;
  assert(arena != 0);
  arena->next = (intptr)(buffer.ptr + sizeof(ArenaAllocator));
  arena->end = (intptr)(buffer.ptr + buffer.size);
  return arena;
}

intptr arena_alloc(ArenaAllocator* arena, Size size) {
  intptr ptr = (intptr)arena->next;
  arena->next += size;
  return ptr;
}
void arena_free(ArenaAllocator* arena, intptr ptr) {
  assert(ptr >= (intptr)arena && ptr <= (intptr)arena->next);
  arena->next = ptr;
}

/* TODO: ArenaAllocator* sub_arena(ArenaAllocator* arena) */
