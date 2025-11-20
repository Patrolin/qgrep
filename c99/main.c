// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"

void main_multicore(Thread t) {
  if (single_core(t)) {
    f64 a = 2.0;
    f64 b = 2.0;
    f64 c = 2.0;
    // fma()
    f64 result = a;
    asm("vfmadd213sd %0, %1, %2"
        : "=x"(result)
        : "0"(result), "x"(b), "x"(c));
    assert(result == 6.0);

    intptr end;
    f64 x = parse_f64(string("0.5e1"), 0, &end);
    printfln1(string("end: %"), intptr, end);
  }
}
