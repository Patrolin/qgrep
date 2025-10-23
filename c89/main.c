// vcvarsall x64
// clang c89/main.c -std=c89 -nostdlib -fuse-ld=radlink -o foo.exe; foo.exe
#include "lib/lib.h"

void _start() {
  AttachConsole(ATTACH_PARENT_PROCESS);
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  const char msg[] = "Hello, World!\n";
  DWORD bytes_written;
  WriteConsoleA(hStdout, msg, sizeof(msg) - 1, &bytes_written, 0);

  // MessageBoxA(0, "Hello, World!", "Hello", MB_OK);

  ExitProcess(0);
}
