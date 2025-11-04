// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/fmt.h"
#include "lib/lib.h"

void bar() {
  println(intptr, 21);
  println(intptr, 3516);
}
void foo() {
#if 0
  String msg = string("Hello, world!");
  intptr b = msg.size;
  println(intptr, b);
  println(String, msg);
  println(intptr, 123);
  bar();
  println(intptr, 47);
  assert(false);
#else
  String format = string("x: %, y: %, z: %\n");
  intptr x = 13;
  intptr y = 23;
  intptr z = 37;

  intptr _autogen_max_size = sprint_size_String(format) + CONCAT(sprint_size_, intptr)(x) + CONCAT(sprint_size_, intptr)(y) + CONCAT(sprint_size_, intptr)(z);
  byte _autogen_buffer[_autogen_max_size];
  byte* _autogen_ptr_end = &_autogen_buffer[_autogen_max_size];
  // intptr size = sprintf1(_autogen_ptr_end, format, intptr, x);
  // intptr size = sprintf2(_autogen_ptr_end, format, intptr, x, intptr, y);
  intptr size = sprintf3(_autogen_ptr_end, format, intptr, x, intptr, y, intptr, z);

  String msg = {_autogen_ptr_end - size, size};
  print_String(msg);
  assert(false);
#endif
}
void main_multicore() {
  foo();
}
