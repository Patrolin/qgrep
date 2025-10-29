// vcvarsall x64
// rm foo.rdi -ErrorAction SilentlyContinue; clang c99/main.c -std=c99 -nostdlib -O0 -g -fuse-ld=radlink -o foo.exe && foo.exe
// raddbg foo.exe
// clang c89/main.c -std=c99 -nostdlib -O2 -fuse-ld=radlink -o foo.exe && foo.exe
#include "lib/lib.h"

void bar() {
  println(intptr, 11);
  println(intptr, 6516);
}
void foo() {
  println(String, string("Hello, world!"));
  println(intptr, 123);
  bar();
  println(intptr, 17);
}
void main_multicore() {
  foo();
}
