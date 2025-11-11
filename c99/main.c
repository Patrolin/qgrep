// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"

void main_multicore(Thread t) {
  if (single_core(t)) {
    printfln1(string("single: thread %"), u32, t);
  }
  barrier(t);
  if (split_threads(t, 4)) {
    printfln1(string("split: thread %"), u32, t);
  } else {
    printfln1(string("else: thread %"), u32, t);
  }
}
