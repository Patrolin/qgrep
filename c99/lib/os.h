#pragma once
#include "definitions.h"

#if OS_WINDOWS
#pragma comment(linker, "/ENTRY:_start")

#define OS_WINDOWS_APP 0
#define OS_WINDOWS_CONSOLE (!OS_WINDOWS_APP)

#if OS_WINDOWS_APP
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#else
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

typedef bool BOOL;
typedef u32 DWORD;
typedef uintptr HANDLE;

// common
ENUM(DWORD, ConsoleHandle){
    STD_INPUT_HANDLE = -10,
    STD_OUTPUT_HANDLE = -11,
    STD_ERROR_HANDLE = -12,
};
/* TODO: this is currently a global, what do we call this enum? */
const DWORD ATTACH_PARENT_PROCESS = (DWORD)-1;

ENUM(DWORD, CodePage){
    CP_UTF8 = 65001,
};
ASSERT(sizeof(CodePage) == 4);
ASSERT(sizeof(CP_UTF8) == 4);

#pragma comment(lib, "kernel32.lib")
foreign BOOL AttachConsole(DWORD process_id);
foreign HANDLE GetStdHandle(DWORD std_handle);
foreign BOOL SetConsoleOutputCP(CodePage code_page);
foreign BOOL WriteConsoleA(HANDLE console_handle, const byte *buffer,
                           DWORD chars_to_write, DWORD *chars_written,
                           rawptr reserved);
foreign void ExitProcess(CUINT exit_code);

#elif OS_LINUX
ASSERT(false);
#else
ASSERT(false);
#endif
