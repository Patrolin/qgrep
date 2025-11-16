// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"

void main_multicore(Thread t) {
  if (single_core(t)) {
    intptr end;
    f64 x = parse_f64(string("0.5e1"), 0, &end);
    printfln1(string("end: %"), intptr, end);
  }
}
