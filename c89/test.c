// vcvarsall x64
// clang c89/test.c -std=c89 -fuse-ld=radlink -o test.exe; test.exe
#include "lib/lib_definitions.h"
#include <windows.h>
#include <stdio.h>
#include "lib/fmt.h"

int main() {
  int v = foo();
  printf("v: %llu\n", v);
}
int foo() {
  StackAllocator stack = STACK_ALLOCATOR();

  printf("stack.start: %llu\n", stack.start);
  printf("stack.used: %llu\n", stack.used);
  printf("stack.capacity: %llu\n\n", stack.capacity);
  byte* ptr = STACK_ALLOC(stack, 4);
  printf("ptr: %llu\n", ptr);
  printf("stack.start: %llu\n", stack.start);
  printf("stack.used: %llu\n", stack.used);
  printf("stack.capacity: %llu\n\n", stack.capacity);
  *ptr = 17;
  printf("*ptr: %llu\n", *ptr);
  return 13;
}
