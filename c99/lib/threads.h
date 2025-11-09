#pragma once
#include "definitions.h"
#include "os.h"
#include "mem.h"
#include "process.h"

// shared
DISTINCT(uintptr, Thread);
typedef struct {
} ThreadInfo;
ASSERT(sizeof(ThreadInfo) <= ARCH_MIN_CACHE_LINE_SIZE);

typedef void RunProc(Thread t);
RunProc main_multicore;

CUINT thread_entry(rawptr param) {
  main_multicore((Thread)param);
  /* TODO: barrier() before exiting */
  return 0;
}

ThreadInfo *global_thread_infos;
void start_threads() {
  uintptr logical_core_count;
#if RUN_SINGLE_THREADED
  logical_core_count = 1;
#else
  #if OS_WINDOWS
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  logical_core_count = info.dwNumberOfProcessors;
  #else
  assert(false);
  #endif
#endif
  assert(logical_core_count > 0);
  global_thread_infos = arena_alloc2(global_arena, ThreadInfo, logical_core_count);
  for (Thread t = 1; t < logical_core_count; t++) {
#if OS_WINDOWS
    assert(CreateThread(0, 0, thread_entry, (rawptr)t, STACK_SIZE_PARAM_IS_A_RESERVATION, 0) != 0);
#else
    assert(false);
#endif
  }
  thread_entry(0);
}
