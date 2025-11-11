#pragma once
#include "definitions.h"
#include "os.h"
#include "mem.h"
#include "process.h"

// shared
DISTINCT(u32, Thread);
typedef align(32) struct {
  /* NOTE: the barriers could be u8 */
  u32 barrier;
  u32 join_barrier;
  Thread threads_start;
  Thread threads_end;
  u32 is_first_counter;
  Thread was_first_thread;
  u32 is_last_counter;
  u32 join_counter;
} ThreadData;
ASSERT(sizeof(ThreadData) == 32);
ASSERT(alignof(ThreadData) == 32);
typedef struct {
  u32 logical_core_count;
  u64* values;
  ThreadData thread_data[];
} ThreadInfo;
ASSERT(sizeof(ThreadInfo) == 32);
ASSERT(alignof(ThreadInfo) == 32);
ThreadInfo* global_thread_infos;

// entry
forward_declare void main_multicore(Thread t);
forward_declare void join_threads(Thread t, Thread threads_start, Thread threads_end);
CUINT thread_entry(rawptr param) {
  Thread t = (Thread)(uintptr)param;
  main_multicore(t);
  join_threads(t, 0, global_thread_infos->logical_core_count);
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
  /* TODO: align the allocation, so that ThreadDatas are aligned to 32 bits */
  global_thread_infos = arena_alloc_flexible(global_arena, ThreadInfo, ThreadData, logical_core_count);
  global_thread_infos->logical_core_count = logical_core_count;
  for (Thread t = 0; t < logical_core_count; t++) {
    global_thread_infos->thread_data[t].threads_end = logical_core_count;
    if (t > 0) {
#if OS_WINDOWS
      assert(CreateThread(0, 0, thread_entry, (rawptr)(uintptr)t, STACK_SIZE_PARAM_IS_A_RESERVATION, 0) != 0);
#else
      assert(false);
#endif
    }
  }
  thread_entry(0);
}

// multi-core
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

void barrier(Thread t) {
  u32 threads_start = global_thread_infos->thread_data[t].threads_start;
  u32 threads_end = global_thread_infos->thread_data[t].threads_end;
  ThreadData* shared_data = &global_thread_infos->thread_data[threads_start];
  u32 thread_count = threads_end - threads_start;

  bool not_last = atomic_add_fetch(&shared_data->is_last_counter, 1) % thread_count != 0;
  if (not_last) {
    wait_on_address(&shared_data->barrier, shared_data->barrier);
  } else {
    /* NOTE: reset counters in case we have a non-power-of-two number of threads */
    shared_data->is_first_counter = 0;
    shared_data->is_last_counter = 0;
    shared_data->barrier += 1;
    wake_all_on_address(&shared_data->barrier);
  }
}
// single-core
bool single_core(Thread t) {
  u32 threads_start = global_thread_infos->thread_data[t].threads_start;
  u32 threads_end = global_thread_infos->thread_data[t].threads_end;
  ThreadData* shared_data = &global_thread_infos->thread_data[threads_start];
  u32 thread_count = threads_end - threads_start;

  u32 prev_i = atomic_fetch_add(&shared_data->is_first_counter, 1);
  bool is_first = prev_i % thread_count == 0;
  if (is_first) {
    shared_data->was_first_thread = t;
  }
  return is_first;
}
void barrier_scatter(Thread t, u64* value) {
  Thread threads_start = global_thread_infos->thread_data[t].threads_start;
  ThreadData* shared_data = &global_thread_infos->thread_data[threads_start];
  u64* shared_value = &global_thread_infos->values[threads_start];
  if (t == shared_data->was_first_thread) {
    *shared_value = *value;
  }
  barrier(t); /* NOTE: make sure the scatter thread has written the data */
  *value = *shared_value;
  barrier(t); /* NOTE: make sure all threads have read the data */
}
ThreadData* barrier_gather(Thread t, u64 value) {
  global_thread_infos->values[t] = value;
  barrier(t); /* NOTE: make sure all threads have written their data */
  return global_thread_infos->thread_data;
}
// split threads
bool split_threads(Thread t, u32 n) {
  // barrier() + modify threads
  u32 threads_start = global_thread_infos->thread_data[t].threads_start;
  u32 threads_end = global_thread_infos->thread_data[t].threads_end;
  ThreadData* shared_data = &global_thread_infos->thread_data[threads_start];
  u32 thread_count = threads_end - threads_start;
  Thread threads_split = threads_start + n;

  bool not_last = atomic_add_fetch(&shared_data->is_last_counter, 1) % thread_count != 0;
  if (not_last) {
    wait_on_address(&shared_data->barrier, shared_data->barrier);
  } else {
    // modify threads
    for (Thread i = threads_start; i < threads_end; i++) {
      ThreadData* thread_data = &global_thread_infos->thread_data[i];
      if (i < threads_split) {
        thread_data->threads_end = threads_split;
      } else {
        thread_data->threads_start = threads_split;
        thread_data->threads_end = threads_end;
      }
    }
    /* NOTE: reset counters in case we have a non-power-of-two number of threads */
    ThreadData* split_data = &global_thread_infos->thread_data[threads_split];
    split_data->is_first_counter = 0;
    split_data->is_last_counter = 0;
    shared_data->is_first_counter = 0;
    shared_data->is_last_counter = 0;
    shared_data->barrier += 1;
    // -modify threads
    wake_all_on_address(&shared_data->barrier);
  }
  return t < threads_split;
}
void join_threads(Thread t, Thread threads_start, Thread threads_end) {
  ThreadData* shared_data = &global_thread_infos->thread_data[threads_start];
  u32 thread_count = threads_end - threads_start;
  u32 join_counter = atomic_add_fetch(&shared_data->join_counter, 1);
  if (join_counter % thread_count != 0) {
    wait_on_address(&shared_data->join_barrier, shared_data->join_barrier);
  } else {
    for (Thread i = threads_start; i < threads_end; i++) {
      ThreadData* thread_data = &global_thread_infos->thread_data[i];
      thread_data->threads_start = threads_start;
      thread_data->threads_end = threads_end;
    }
    shared_data->join_barrier += 1;
    wake_all_on_address(&shared_data->join_barrier);
  }
}
