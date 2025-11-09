// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/definitions.h"
#include "lib/fmt.h"
#include "lib/lib.h"
#include "lib/process.h"
#include "lib/threads.h"

ArenaAllocator* shared_arena;
forward_declare void main_multicore(Thread t);

void start() {
  Bytes buffer = page_reserve(VIRTUAL_MEMORY_TO_RESERVE);
  shared_arena = arena_allocator(buffer);
#if RUN_SINGLE_THREADED
  run_multicore(main_multicore, 1);
#else
  run_multicore(main_multicore, 0);
#endif
}

void main_multicore(Thread t) {
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
