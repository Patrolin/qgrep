#pragma once
#include "definitions.h"
#include "process.h"

// shared
typedef void MainProc();
void main_multicore();
void run_multicore(MainProc proc, intptr thread_count) {
  /* TODO: get thread count */
  proc();
}
void _start() {
  init_console();
  main_multicore();
  /* TODO: run multicore */
  // run_multicore(main_multicore, 1);
  /* TODO: mfence() here? */
  exit_process(0);
}
