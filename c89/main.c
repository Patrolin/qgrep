// vcvarsall x64
// clang c89/main.c -std=c89 -nostdlib -fuse-ld=radlink -o foo.exe; foo.exe
#include "lib/lib.h"

void main_multicore() {
  init_console();

  PRINT(String, STRING("ayaya.1\n"));
  StackAllocator stack = STACK_ALLOCATOR();
  PRINT(String, STRING("ayaya.2\n"));
  String msg = STACK_PRINT(stack, String, msg);
  PRINT(String, msg);

  /* TODO: mfence() here? */
  ExitProcess(0);
}
