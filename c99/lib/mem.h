#pragma once
#include "definitions.h"
#include "os.h"

// page alloc
#if OS_WINDOWS
ExceptionResult _page_fault_handler(_EXCEPTION_POINTERS* exception_info) {
  EXCEPTION_RECORD* exception = exception_info->ExceptionRecord;
  DWORD exception_code = exception->ExceptionCode;
  if (exception_code == EXCEPTION_ACCESS_VIOLATION) {
    uintptr ptr = exception->ExceptionInformation[1];
    intptr page_ptr = (intptr)ptr & ~(intptr)(OS_PAGE_SIZE - 1);
    intptr commited_ptr = VirtualAlloc(page_ptr, OS_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
    return page_ptr != 0 && commited_ptr != 0 ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_EXECUTE_HANDLER;
  }
  return EXCEPTION_EXECUTE_HANDLER;
}
#else
// ASSERT(false);
#endif

void init_page_fault_handler() {
#if OS_WINDOWS
  SetUnhandledExceptionFilter(_page_fault_handler);
#else
  // ASSERT(false);
#endif
}

Bytes page_reserve(Size size) {
  Bytes buffer;
  buffer.size = size;
#if OS_WINDOWS
  buffer.ptr = (byte*)VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
  assert(buffer.ptr != 0);
#elif OS_LINUX
  buffer.ptr = (byte*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert((intptr)buffer.ptr != -1);
#else
  assert(false);
#endif
  return buffer;
}
void page_free(intptr ptr) {
#if OS_WINDOWS
  assert(VirtualFree(ptr, 0, MEM_RELEASE));
#elif OS_LINUX
  assert(munmap(ptr, 0) == 0);
#else
  assert(false);
#endif
}

// copy
always_inline void zero(byte* ptr, Size size) {
  for (intptr i = 0; i < size; i++) {
    ptr[size] = 0;
  }
}

// locks
DISTINCT(bool, Lock);
bool get_lock_or_false(Lock* lock) {
  Lock expected = false;
  return atomic_compare_exchange_weak(lock, &expected, true);
}
void get_lock(Lock* lock) {
  Lock expected = false;
  while (!expected) {
    expected = atomic_compare_exchange_weak(lock, &expected, true);
  }
}
#define get_lock_assert_single_threaded(lock) assert(get_lock_or_false(lock))
void release_lock(Lock* lock) {
  atomic_store(lock, false);
}

// IWYU pragma: begin_exports
#include "mem_arena.h"
// IWYU pragma: end_exports
