// vcvarsall x64
// clang c89/main.c -std=c89 -nostdlib -fuse-ld=radlink -o foo.exe; foo.exe
// raddbg foo.exe
#include "lib/lib.h"

[[clang::optnone]]
void foo() {
  StackAllocator stack = STACK_ALLOCATOR();
  String msg = STRING("Hello, world!");
  String msg2 = STACK_PRINT(stack, String, msg);
  PRINT(String, msg2);
}
void main_multicore() {
  foo();
}
