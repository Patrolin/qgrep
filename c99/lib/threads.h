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

noreturn _startup() {
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
  #if 1
/* NOTE: naked attribute for correctness, but we don't really need it,
  since we have to align manually either way... */
naked noreturn _start() {
  ALIGN_STACK_POINTER();
  CALL(_startup);
}
  #else
noreturn _start() {
  ALIGN_STACK_POINTER();
  _startup();
}
  #endif
#endif
