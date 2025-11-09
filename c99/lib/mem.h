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
#else
  assert(false);
#endif
  assert(buffer.ptr != 0);
  return buffer;
}
void page_free(intptr ptr) {
#if OS_WINDOWS
  assert(VirtualFree(ptr, 0, MEM_RELEASE));
#else
  assert(false);
#endif
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
void release_lock(Lock* lock) {
  atomic_store(lock, false);
}

#include "mem_arena.h" /* IWYU pragma: keep */
