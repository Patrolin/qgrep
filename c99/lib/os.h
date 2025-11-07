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

typedef bool BOOL;
typedef u32 DWORD;
DISTINCT(uintptr, Handle);
DISTINCT(Handle, FileHandle);
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
