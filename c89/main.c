// vcvarsall x64
// clang c89/main.c -std=c89 -nostdlib -fuse-ld=radlink -o foo.exe; foo.exe
#include "lib/lib.h"

void main_multicore() {
  init_console();
  String msg = STRING("Hello, World!\n");
  // String msg2 = SBPRINT(String, msg);
  PRINT(String, msg);
  /* TODO: mfence() here? */
  ExitProcess(0);
}
