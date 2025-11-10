// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"
#include "lib/threads.h"

void main_multicore(Thread t) {
  if (split_thread(t)) {
    println(String, string("single"));
  }
  barrier();
  if (split_threads(4)) {
    println(String, string("multi"));
  }
  barrier();
  printfln1(string("- thread %"), uintptr, t);
}
