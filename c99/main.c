// vcvarsall x64
// rm foo.rdi -ErrorAction SilentlyContinue; clang c99/main.c -std=c99 -nostdlib -O0 -g -fuse-ld=radlink -o foo.exe && foo.exe
// raddbg foo.exe
// clang c89/main.c -std=c99 -nostdlib -O2 -fuse-ld=radlink -o foo.exe && foo.exe
#include "lib/lib.h"

void foo() {
  println(String, string("Hello, world!"));
  println(intptr, 123);
  println(intptr, 17);
  println(intptr, 11);
  println(intptr, 6516);
}
void main_multicore() {
  foo();
}
