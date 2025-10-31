// vcvarsall x64
// ./run-debug
#include "lib/lib.h"

void bar() {
  println(intptr, 21);
  println(intptr, 3516);
}
void foo() {
  String msg = string("Hello, world!");
  intptr b = msg.size;
  println(intptr, b);
  println(String, msg);
  println(intptr, 123);
  bar();
  println(intptr, 47);
}
void main_multicore() {
  foo();
}
