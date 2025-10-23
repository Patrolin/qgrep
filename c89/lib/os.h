#if OS_WINDOWS
#pragma comment(linker, "/ENTRY:_start")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
// #pragma comment(linker, "/SUBSYSTEM:CONSOLE")

typedef bool BOOL;
typedef u32 DWORD;
typedef uintptr HANDLE;

// common
const DWORD STD_OUTPUT_HANDLE = -11;
const DWORD ATTACH_PARENT_PROCESS = -1;

LIB(kernel32.lib)
foreign BOOL AttachConsole(DWORD process_id);
foreign HANDLE GetStdHandle(DWORD std_handle);
foreign BOOL WriteConsole(HANDLE console_handle, const rawptr buffer, DWORD chars_to_write, DWORD* chars_written, rawptr reserved);
foreign void ExitProcess(CUINT exit_code);

#elif OS_LINUX
#endif
