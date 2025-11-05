// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/definitions.h"
#include "lib/fmt.h"
#include "lib/lib.h"

void bar() {
  println(intptr, 21);
  println(intptr, 3516);
}
void foo() {
#if 0
  const char *line = __FILE__ ":" STR(__LINE__) " " STR(false) "\n";
  write(STDOUT, (char *)line, 5);
#elif 0
  String format = string("x: %, y: %, z: %\n");
  write(STDOUT, format.ptr, format.size);

  intptr max_size = format.size;
  byte buffer[max_size];
  byte* ptr_end = &buffer[max_size];
#elif 0
  String msg = string("Hello, world!");
  intptr b = msg.size;
  println(intptr, b);
  println(String, msg);
  println(intptr, 123);
  bar();
  println(intptr, 47);
  assert(false);
#else
  String format = string("xá: %, y: %, z: %\n");
  intptr x = 13;
  intptr y = 23;
  intptr z = 37;

  intptr max_size = sprint_size4(String, format, intptr, x, intptr, y, intptr, z);
  byte buffer[max_size];
  byte* ptr_end = &buffer[max_size];
  // intptr size = sprintf1(ptr_end, format, intptr, z);
  // intptr size = sprintf2(ptr_end, format, intptr, y, intptr, z);
  intptr size = sprintf3(ptr_end, format, intptr, x, intptr, y, intptr, z);

  String msg = {ptr_end - size, size};
  print_String(msg);
  assert(false);
#endif
}
void main_multicore() {
  foo();
}
