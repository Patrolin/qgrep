#pragma once
#include "definitions.h"
#include "os.h"

#if OS_WINDOWS
ConsoleHandle stdin, stdout, stderr;
void init_console() {
  #if OS_WINDOWS_APP
  AttachConsole(ATTACH_PARENT_PROCESS);
  #endif
  stdin = GetStdHandle(STD_INPUT_HANDLE);
  stdout = GetStdHandle(STD_OUTPUT_HANDLE);
  stderr = GetStdHandle(STD_ERROR_HANDLE);
}
#elif OS_LINUX
void init_console() { assert(false); };
#else
CASSERT(false);
#endif

noreturn void exit_process(CINT exit_code) {
#if OS_WINDOWS
  ExitProcess((CUINT)exit_code);
#elif OS_LINUX
  _exit(exit_code);
#else
  ASSERT(false);
#endif
  for (;;);
}

noreturn void abort() {
  /* NOTE: technically you should signal abort on linux, but eh... */
  exit_process(1);
}
