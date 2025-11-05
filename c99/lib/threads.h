#pragma once
#include "definitions.h"
#include "process.h"

// shared
typedef void MainProc();
void main_multicore();
void run_multicore(MainProc proc, intptr thread_count) {
  /* TODO: get thread count if thread_count==0 */
  /* TODO: run multicore */
  proc();
}

void _startup() {
  init_console();
  run_multicore(main_multicore, 1);
  /* TODO: mfence() here? */
  exit_process(0);
}

#if HAS_CRT
CINT main() {
  _startup();
}
#else
void _start() {
  ALIGN_STACK_POINTER();
  _startup();
}
#endif
