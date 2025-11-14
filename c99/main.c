// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"

void main_multicore(Thread t) {
  if (single_core(t)) {
    intptr end;
    f64 x = parse_f64_decimal(string("0.55"), 0, &end);
    printfln1(string("thread %: x=%"), u32, t);
  }
}
