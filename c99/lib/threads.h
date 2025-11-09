#pragma once
#include "definitions.h"

// shared
DISTINCT(intptr, Thread);
typedef struct {
  /* TODO: alloc thread infos */
} ThreadInfo;
ASSERT(sizeof(ThreadInfo) <= ARCH_MIN_CACHE_LINE_SIZE);
ASSERT(ARCH_MIN_CACHE_LINE_SIZE % alignof(ThreadInfo) == 0);

typedef void RunProc(Thread t);
void run_multicore(RunProc run_proc, intptr thread_count) {
  /* TODO: get thread count if thread_count==0 */
  /* TODO: run multicore */
  run_proc(0);
}
