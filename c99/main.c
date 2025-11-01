// vcvarsall x64
// ./run-debug
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/definitions.h"
#include "lib/lib.h"

void bar() {
  println(intptr, 21);
  println(intptr, 3516);
}
void foo() {
#if 0
  String msg = string("Hello, world!\n");
  write(STDOUT, msg.ptr, msg.size);
#else
  const char* foo = (const char*)(0x555555557ef0);
  String msg = string("Hello, world!\n");
  write(STDOUT, msg.ptr, msg.size);
  mfence();
  println(String, msg);
#endif
  /*
  String msg = string("Hello, world!");
  intptr b = msg.size;
  println(intptr, b);
  println(String, msg);
  println(intptr, 123);
  bar();
  println(intptr, 47);
  assert(false);
  */
}
void main_multicore() {
  foo();
}
