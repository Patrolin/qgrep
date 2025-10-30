// vcvarsall x64
// rm foo.rdi -ErrorAction SilentlyContinue; clang c99/main.c -std=c99 -nostdlib -O0 -Werror -Wvla -Wconversion -g -fuse-ld=radlink -o foo.exe && foo.exe
// raddbg foo.exe
// clang c99/main.c -std=c99 -nostdlib -Werror -Wvla -Wconversion -O2 -fuse-ld=radlink -o foo.exe && foo.exe
#include "lib/lib.h"

void bar(intptr size) {
  println(intptr, 11);
  println(intptr, 6516);
}
void foo() {
  println(String, string("Hello, world!"));
  println(intptr, 123);
  bar(3);
  println(intptr, 17);
}
void main_multicore() {
  foo();
}
