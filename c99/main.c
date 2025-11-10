// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"
#include "lib/threads.h"

void main_multicore(Thread t) {
  if (sync_is_first(t)) {
    printfln1(string("thread %"), uintptr, t);
  }
}
