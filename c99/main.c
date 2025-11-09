// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"

void main_multicore(Thread t) {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  printfln1(string("info.dwNumberOfProcessors: %"), u32, info.dwNumberOfProcessors);
}
