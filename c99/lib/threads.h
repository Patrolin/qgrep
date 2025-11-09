#pragma once
#include "definitions.h"
#include "os.h"

// shared
DISTINCT(intptr, Thread);
typedef struct {
} ThreadInfo;
ASSERT(sizeof(ThreadInfo) <= ARCH_MIN_CACHE_LINE_SIZE);
ASSERT(ARCH_MIN_CACHE_LINE_SIZE % alignof(ThreadInfo) == 0);

typedef void RunProc(Thread t);
RunProc main_multicore;
void start_threads() {
#if RUN_SINGLE_THREADED
  main_multicore(0);
#else
  intptr logical_core_count;
  #if OS_WINDOWS
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  logical_core_count = info.dwNumberOfProcessors;
  #else
  assert(false);
  #endif
  /* TODO: alloc thread infos */
  /* TODO: start other threads */
  main_multicore(0);
#endif
}
