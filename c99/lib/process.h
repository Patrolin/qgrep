#pragma once
#include "definitions.h"
#include "os.h"

#if OS_WINDOWS
void init_console() {
  #if OS_WINDOWS_APP
  AttachConsole(ATTACH_PARENT_PROCESS);
  #endif
}
#elif OS_LINUX
void init_console() {};
#else
CASSERT(false);
#endif

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
