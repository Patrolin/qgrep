// vcvarsall x64
// clang c89/main.c -std=c89 -nostdlib -fuse-ld=radlink -o foo.exe; foo.exe
#include "lib/lib.h"

void main_multicore() {
  const char msg[] = "Hello, World!\n";
  DWORD bytes_written;
  WriteConsoleA(stdout, msg, sizeof(msg) - 1, &bytes_written, 0);
  print(bytes_written);
}
