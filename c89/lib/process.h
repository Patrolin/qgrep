#pragma once
#include "definitions.h"
#include "os.h"

#if OS_WINDOWS
HANDLE stdin, stdout, stderr;
void init_console() {
  AttachConsole(ATTACH_PARENT_PROCESS);
  stdin = GetStdHandle(STD_INPUT_HANDLE);
  stdout = GetStdHandle(STD_OUTPUT_HANDLE);
  stderr = GetStdHandle(STD_ERROR_HANDLE);
}
#elif OS_LINUX
void init_console(){};
#else
CASSERT(false);
#endif
