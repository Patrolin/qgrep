#pragma once
#include "definitions.h"
#include "process.h"

// shared
typedef void MainProc();
void main_multicore();

#if HAS_CRT
CINT main() {
  init_console();
  main_multicore();
  exit_process(0);
}
#else
void run_multicore(MainProc proc, intptr thread_count) {
  /* TODO: get thread count */
  proc();
}
void _start() {
  asm volatile("andq $-16, %%rsp" ::: "rsp");
  init_console();
  main_multicore();
  /* TODO: run multicore */
  // run_multicore(main_multicore, 1);
  /* TODO: mfence() here? */
  exit_process(0);
}
#endif
