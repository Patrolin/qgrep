// shared
typedef void MainProc();
void main_multicore();
void run_multicore(MainProc proc, intptr thread_count) {
  /* TODO: get thread count */
  /* TODO: run multicore */
  proc();
}
void _start() {
  init_console();
  run_multicore(main_multicore, 1);
  /* TODO: mfence() here? */
  ExitProcess(0);
}
