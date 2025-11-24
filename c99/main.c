// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"
#include <stdio.h>
_Check_return_ _ACRTIMP double __cdecl nan(_In_ char const* _X);

void main_multicore(u32 t) {
  if (single_core(t)) {
    f64 a = 2.0;
    f64 b = 0.1;
    f64 c = 0.1;
    f64 result = fma(a, b, c);
    assert(0.3 != 0.1 + 0.2);
    assert(result == 0.1 + 0.2);

    intptr end;
    f64 x = parse_f64(string("0.5e1"), 0, &end);
    printfln1(string("end: %"), intptr, end);

    printf("strtod(1.0): %g\n", strtod("1.0", 0));
    printf("g: %g\n", 1.0 / 0.0);
    printf("e: %g\n", nan("test"));
    printf("g: %g\n", 1e6);
    printf("g: %g\n", 1e5);

    printf("g: %g\n", 0.1 + 0.2);               // 0.3
    printf("g: %g\n", 0.3);                     // 0.3
    printf("g: %.17g\n", 0.1 + 0.2);            // 0.30000000000000004
    printf("g: %.17g\n", 0.3);                  // 0.29999999999999999
    printf("g: %.17g\n", 0.30000000000000004);  // 0.30000000000000004
    printf("g: %.17g\n", 0.29999999999999999);  // 0.29999999999999999

    printf("g: %g\n", 1e-4);
    printf("g: %g\n", 9e-5);
  }
}
