#pragma once
#include "definitions.h"

// shared
DISTINCT(intptr, Thread);
typedef struct {
  /* TODO: alloc thread infos */
} ThreadInfo;
ASSERT(sizeof(ThreadInfo) <= ARCH_MIN_CACHE_LINE_SIZE);
ASSERT(ARCH_MIN_CACHE_LINE_SIZE % alignof(ThreadInfo) == 0);

forward_declare void main_multicore(Thread t);
void run_multicore(intptr thread_count) {
  /* TODO: get thread count if thread_count==0 */
  /* TODO: run multicore */
  main_multicore(0);
}
