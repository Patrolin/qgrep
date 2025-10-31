// vcvarsall x64
// ./run-debug
#include "lib/lib.h"

void bar() {
  println(intptr, 11);
  println(intptr, 6516);
}
void foo() {
  print(String, string("Hello, world!"));
  println(intptr, 123);
  bar();
  println(intptr, 17);
}
void main_multicore() {
  foo();
}
