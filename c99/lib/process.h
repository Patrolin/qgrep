#pragma once
#include "definitions.h"
#include "mem_arena.h"
#include "os.h"

#define VIRTUAL_MEMORY_TO_RESERVE OS_MIN_STACK_SIZE

forward_declare void run_multicore(intptr thread_count);
forward_declare void init_page_fault_handler();
forward_declare Bytes page_reserve(Size size);

// init
void init_console() {
#if OS_WINDOWS
  SetConsoleOutputCP(CP_UTF8);
#else
  // ASSERT(false);
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
ArenaAllocator* shared_arena;
noreturn _startup() {
  init_console();
  init_page_fault_handler();
  Bytes buffer = page_reserve(VIRTUAL_MEMORY_TO_RESERVE);
  shared_arena = arena_allocator(buffer);
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
