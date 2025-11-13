// vcvarsall x64
// ./run
// raddbg foo.exe
// gdb foo-linux-x64
#include "lib/all.h"
#include "lib/threads.h"

void main_multicore(Thread t) {
  u64 x;
  if (single_core(t)) {
    printfln2(string("thread %: single, waiting: %"), u32, t, u32, global_threads->thread_infos[0].is_last_counter);  // print on one thread
    x = 13;
  }
  barrier_scatter(t, &x);
  printfln2(string("thread %: x=%"), u32, t, u64, x);  // print on all threads

  if (barrier_split_threads(t, 4)) {
    printfln1(string("split: thread %"), u32, t);  // print on 4 threads
  } else {
    printfln1(string("else: thread %"), u32, t);  // print on remaining threads
  }
}
