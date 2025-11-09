#pragma once
#include "definitions.h"
#include "os.h"

// init
void init_console() {
#if OS_WINDOWS
  SetConsoleOutputCP(CP_UTF8);
#else
  // ASSERT(false);
#endif
}
forward_declare void init_page_fault_handler();
forward_declare void start();
forward_declare noreturn exit_process(CINT exit_code);

// entry
noreturn _start2() {
  init_console();
  init_page_fault_handler();
  start();
  exit_process(0);
}
#if HAS_CRT
CINT main() {
  _start2();
}
#else
  #if 1
/* NOTE: naked attribute for correctness, but we don't really need it,
  since we have to align the stack pointer manually either way... */
naked noreturn _start() {
  ALIGN_STACK_POINTER();
  CALL(_start2);
}
  #else
noreturn _start() {
  ALIGN_STACK_POINTER();
  _start2();
}
  #endif
#endif

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
