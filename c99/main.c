#include "lib/all.h"
#include "lib/fmt.h"
#include "lib/threads.h"

void main_multicore(Thread t) {
  if (single_core(t)) {
    printfln1(string("x: %"), intptr, 17);
  }
}
