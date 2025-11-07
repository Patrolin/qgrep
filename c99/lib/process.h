#pragma once
#include "definitions.h"
#include "os.h"
#include "threads.h"

// init
void init_console() {
#if OS_WINDOWS
  SetConsoleOutputCP(CP_UTF8);
#endif
}

// exit
noreturn exit_process(CINT exit_code) {
#if OS_WINDOWS
  ExitProcess((CUINT)exit_code);
#elif OS_LINUX
  _exit(exit_code);
#else
  ASSERT(false);
#endif
  for (;;);
}
noreturn abort() {
  /* NOTE: technically you should signal abort on linux, but eh... */
  exit_process(1);
}

// entry
noreturn _startup() {
  init_console();
#if RUN_SINGLE_THREADED
  run_multicore(1);
#else
  run_multicore(0);
#endif
  exit_process(0);
}
#if HAS_CRT
CINT main() {
  _startup();
}
#else
  #if 1
/* NOTE: naked attribute for correctness, but we don't really need it,
  since we have to align the stack pointer manually either way... */
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
