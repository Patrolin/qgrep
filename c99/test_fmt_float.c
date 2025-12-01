// clang c99/test_fmt_float.c -Wno-gcc-compat -march=native -masm=intel -std=gnu99 -nostdlib -mno-stack-arg-probe -Werror -Wconversion -Wnullable-to-nonnull-conversion -fno-signed-char -O2 -flto -g -fuse-ld=lld "-Wl,/STACK:0x100000" -o test_fmt_float.exe && test_fmt_float.exe
#include "lib/all.h"
#include "lib/fmt_float.h"
#include "lib/threads.h"

void main_multicore(Thread t) {
  u64* runs_ptr;
  u64* succeeded_ptr;
  if (single_core(t)) {
    runs_ptr = arena_alloc(global_arena, u64);
    succeeded_ptr = arena_alloc(global_arena, u64);
  }
  barrier_scatter(t, &runs_ptr);
  barrier_scatter(t, &succeeded_ptr);

  u64 current_run;
  u64 max_runs = MAX(u32);
  while (true) {
    current_run = atomic_fetch_add(runs_ptr, 1);
    if (expect_small(current_run >= max_runs)) break;
    u64 value = random_u64(current_run);
    bool ok = false;
    // condition
    f64 value_f64 = bitcast(value, u64, f64);
    value_f64 = 0.3;
    byte buffer[sprint_size_f64(value)];
    sprint_f64_libc(value_f64, buffer + sprint_size_f64(value));

    f64 new_value = strtod(buffer, 0);
    ok = new_value == value_f64 || isnan(value_f64);
    if (expect_small(!ok)) {
      printf("\n%.17g\n%s\n", value_f64, buffer);
      printfln2(string("% -> %"), uhex, value, uhex, bitcast(new_value, f64, u64));
      assert(false);
    }
    // -condition
    test(ok, current_run, value, succeeded_ptr);
    print_test_progress(t == 0, current_run, max_runs);
  }
  barrier(t);

  print_tests_done(t, atomic_load(succeeded_ptr), max_runs);
}
