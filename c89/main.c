// vcvarsall x64
// clang c89/main.c -std=c89 -nostdlib -fdebug-macro -O0 -fuse-ld=radlink -o foo.exe; foo.exe
// raddbg foo.exe
// clang c89/main.c -std=c89 -nostdlib -O2 -fuse-ld=radlink -o foo.exe; foo.exe
#include "lib/lib.h"

void foo() {
  StackAllocator stack = stack_allocator();
  String msg = string("Hello, world!\n");
  String msg2 = stack_print(stack, String, msg);
  print(String, msg2);
}
void main_multicore() {
  foo();
}
