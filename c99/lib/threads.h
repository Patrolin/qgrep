#pragma once
#include "definitions.h"

// shared
/* TODO: alloc thread infos */
typedef struct {
  intptr index;
} Thread;
ASSERT(sizeof(Thread) <= ARCH_MIN_CACHE_LINE_SIZE);
typedef void MainProc();
void main_multicore();

void run_multicore(MainProc proc, intptr thread_count) {
  /* TODO: get thread count if thread_count==0 */
  /* TODO: run multicore */
  proc();
}
