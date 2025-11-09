#pragma once
#include "definitions.h"

// common
#if OS_WINDOWS
  #pragma comment(linker, "/ENTRY:_start")
  #if RUN_WITHOUT_CONSOLE
    /* NOTE: /SUBSYSTEM:WINDOWS cannot connect to a console without a race condition, or spawning a new window */
    #pragma comment(linker, "/SUBSYSTEM:WINDOWS")
  #else
    #pragma comment(linker, "/SUBSYSTEM:CONSOLE")
  #endif

typedef CINT BOOL;
typedef u64 QWORD;
typedef u32 DWORD;
typedef u16 WORD;
DISTINCT(uintptr, Handle);
DISTINCT(Handle, FileHandle);
  #if ARCH_IS_64_BIT
    #define WINAPI
  #elif ARCH_IS_32_BIT
    #define WINAPI TODO
  #endif
#elif OS_LINUX
  #include "os_linux.h"

/* NOTE: everything is a file on linux */
DISTINCT(CINT, FileHandle);
DISTINCT(FileHandle, Handle);
#else
ASSERT(false);
#endif

// process
#if OS_WINDOWS
ENUM(FileHandle, ConsoleHandleEnum){
    STDIN = -10,
    STDOUT = -11,
    STDERR = -12,
};
ENUM(DWORD, CodePage){
    CP_UTF8 = 65001,
};

  #pragma comment(lib, "kernel32.lib")
foreign BOOL SetConsoleOutputCP(CodePage code_page);
foreign BOOL WriteFile(FileHandle file, const byte* buffer, DWORD buffer_size, DWORD* bytes_written, rawptr overlapped);
foreign void ExitProcess(CUINT exit_code);

#elif OS_LINUX
ENUM(FileHandle, ConsoleHandleEnum){
    STDIN = 0,
    STDOUT = 1,
    STDERR = 2,
};
intptr write(FileHandle file, const byte* buffer, intptr buffer_size) {
  return syscall3(SYS_write, (uintptr)file, (uintptr)buffer, (uintptr)buffer_size);
}
noreturn _exit(CINT return_code) {
  syscall1(SYS_exit, (uintptr)return_code);
  for (;;);
}
#else
ASSERT(false);
#endif

// mem
#if OS_WINDOWS
ENUM(DWORD, ExceptionCode){
    EXCEPTION_ACCESS_VIOLATION = 0xC0000005,
};
  #define EXCEPTION_MAXIMUM_PARAMETERS 15
typedef struct {
  ExceptionCode ExceptionCode;
  DWORD ExceptionFlags;
  struct EXCEPTION_RECORD* ExceptionRecord;
  rawptr ExceptionAddress;
  DWORD NumberParameters;
  uintptr ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD;

typedef struct {
  /* ... */
} CONTEXT;
typedef struct {
  EXCEPTION_RECORD* ExceptionRecord;
  CONTEXT* ContextRecord;
} _EXCEPTION_POINTERS;
ENUM(DWORD, ExceptionResult){
    EXCEPTION_EXECUTE_HANDLER = 1,
    EXCEPTION_CONTINUE_SEARCH = 0,
    EXCEPTION_CONTINUE_EXECUTION = -1,
};
typedef ExceptionResult TOP_LEVEL_EXCEPTION_FILTER(_EXCEPTION_POINTERS* exception);

ENUM(DWORD, AllocTypeFlags){
    MEM_COMMIT = 1 << 12,
    MEM_RESERVE = 1 << 13,
    MEM_DECOMMIT = 1 << 14,
    MEM_RELEASE = 1 << 15,
};
ENUM(DWORD, AllocProtectFlags){
    PAGE_READWRITE = 1 << 2,
};

foreign TOP_LEVEL_EXCEPTION_FILTER* SetUnhandledExceptionFilter(TOP_LEVEL_EXCEPTION_FILTER filter_callback);
foreign intptr VirtualAlloc(intptr address, Size size, AllocTypeFlags type_flags, AllocProtectFlags protect_flags);
foreign BOOL VirtualFree(intptr address, Size size, AllocTypeFlags type_flags);
#endif

// threads
#if OS_WINDOWS
typedef struct {
  union {
    DWORD dwOemId;
    struct {
      WORD wProcessorArchitecture;
      WORD wReserved;
    } DUMMYSTRUCTNAME;
  } DUMMYUNIONNAME;
  DWORD dwPageSize;
  rawptr lpMinimumApplicationAddress;
  rawptr lpMaximumApplicationAddress;
  DWORD* dwActiveProcessorMask;
  DWORD dwNumberOfProcessors;
  DWORD dwProcessorType;
  DWORD dwAllocationGranularity;
  WORD wProcessorLevel;
  WORD wProcessorRevision;
} SYSTEM_INFO;
typedef struct SECURITY_ATTRIBUTES {
  DWORD nLength;
  rawptr lpSecurityDescriptor;
  BOOL bInheritHandle;
} SECURITY_ATTRIBUTES;
typedef DWORD PTHREAD_START_ROUTINE(rawptr param);
ENUM(DWORD, CreateThreadFlags){
    STACK_SIZE_PARAM_IS_A_RESERVATION = 0x00010000,
};
DISTINCT(Handle, ThreadHandle);

foreign void GetSystemInfo(SYSTEM_INFO* lpSystemInfo);
foreign ThreadHandle CreateThread(
    SECURITY_ATTRIBUTES* security,
    Size stack_size,
    PTHREAD_START_ROUTINE start_proc,
    rawptr param,
    DWORD flags,
    DWORD* thread_id);
#else
// ASSERT(false);
#endif

// file
#if OS_WINDOWS
/* TODO: file api */
#elif OS_LINUX
ENUM(CUINT, FileFlags){
    O_WRONLY = 1 << 0,
    O_RDWR = 1 << 1,
    /* create if not exists */
    O_CREAT = 1 << 6,
    /* don't open */
    O_EXCL = 1 << 7,
    /* truncate */
    O_TRUNC = 1 << 9,
    O_DIRECTORY = 1 << 16,
};
intptr open(const byte* path, FileFlags flags, CUINT mode) {
  return syscall3(SYS_open, (uintptr)path, flags, mode);
}
#endif
