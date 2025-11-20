// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"

void main_multicore(Thread t) {
  if (single_core(t)) {
    f64 a = 1.0;
    f64 b = 0.1;
    f64 c = 0.2;
    f64 result = fma(a, b, c);
    assert(0.3 != 0.1 + 0.2);
    assert(result == 0.1 + 0.2);

    intptr end;
    f64 x = parse_f64(string("0.5e1"), 0, &end);
    printfln1(string("end: %"), intptr, end);
  }
}
