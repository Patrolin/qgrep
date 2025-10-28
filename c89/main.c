// vcvarsall x64
// clang c89/main.c -std=c89 -nostdlib -O0 -fuse-ld=radlink -o foo.exe; foo.exe
// raddbg foo.exe
// clang c89/main.c -std=c89 -nostdlib -O2 -fuse-ld=radlink -o foo.exe; foo.exe
#include "lib/lib.h"

void foo() {
  StackAllocator stack = STACK_ALLOCATOR();
  String msg = STRING("Hello, world!\n");
  String msg2 = STACK_PRINT(stack, String, msg);
  PRINT(String, msg2);
}
void main_multicore() {
  foo();
}
