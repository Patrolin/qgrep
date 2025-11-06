// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/definitions.h"
#include "lib/fmt.h"
#include "lib/lib.h"

void foo() {
#if 1
  printfln1(string("ARCH_HAS_NATIVE_BF16: %"), intptr, ARCH_HAS_NATIVE_BF16);
  printfln1(string("ARCH_HAS_NATIVE_F16: %"), intptr, ARCH_HAS_NATIVE_F16);
  printfln2(string("x: %, y: %"), intptr, 13, intptr, 22);
  printfln3(string("x: %, y: %, z: %"), intptr, 13, intptr, 22, intptr, 37);
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
