#pragma once
#include "definitions.h"
#include "os.h"
#include "mem.h"
#include "process.h"

// shared
DISTINCT(u32, Thread);
typedef struct {
  u32 barrier;
  u32 order;
  Thread was_first_thread;
  u32 thread_count;
  u64 values[];
} ThreadInfos;
ASSERT(sizeof(ThreadInfos) == 16);
ThreadInfos* global_thread_infos;

// entry
forward_declare void main_multicore(Thread t);
CUINT thread_entry(rawptr param) {
  main_multicore((Thread)(uintptr)param);
  /* TODO: barrier() before exiting */
  return 0;
}

void start_threads() {
  u32 logical_core_count;
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
  global_thread_infos = arena_alloc_flexible(global_arena, ThreadInfos, u64, logical_core_count);
  global_thread_infos->thread_count = logical_core_count;
  for (Thread t = 1; t < logical_core_count; t++) {
#if OS_WINDOWS
    assert(CreateThread(0, 0, thread_entry, (rawptr)(uintptr)t, STACK_SIZE_PARAM_IS_A_RESERVATION, 0) != 0);
#else
    assert(false);
#endif
  }
  thread_entry(0);
}

// barrier
void wait_on_address(u32* address, u32 not_expected) {
#if OS_WINDOWS
  WaitOnAddress(address, &not_expected, sizeof(not_expected), INFINITE);
#else
  assert(false);
#endif
}
void wake_all_on_address(u32* address) {
#if OS_WINDOWS
  WakeByAddressAll(address);
#else
  assert(false);
#endif
}

void barrier() {
  intptr not_last = atomic_add(&global_thread_infos->order, 1) % global_thread_infos->thread_count != 0;
  if (not_last) {
    wait_on_address(&global_thread_infos->barrier, global_thread_infos->barrier);
  } else {
    global_thread_infos->barrier += 1;
    wake_all_on_address(&global_thread_infos->barrier);
  }
}
bool sync_is_first(Thread t) {
  intptr is_first = atomic_add(&global_thread_infos->order, 1) % global_thread_infos->thread_count == 1;
  if (is_first) {
    global_thread_infos->was_first_thread = t;
  }
  return is_first;
}
void barrier_scatter(Thread t, u64* value) {
  if (t == global_thread_infos->was_first_thread) {
    global_thread_infos->values[0] = *value;
  }
  barrier();
  *value = global_thread_infos->values[0];
  barrier();
}
u64* barrier_gather(Thread t, u64 value) {
  global_thread_infos->values[t] = value;
  barrier();
  return global_thread_infos->values;
}
