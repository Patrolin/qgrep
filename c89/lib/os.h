#if OS_WINDOWS
#pragma comment(linker, "/ENTRY:_start")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
// #pragma comment(linker, "/SUBSYSTEM:CONSOLE")

typedef bool BOOL;
typedef u32 DWORD;
typedef uintptr HANDLE;

// common
ENUM(DWORD, ConsoleHandle){
    STD_INPUT_HANDLE = -10,
    STD_OUTPUT_HANDLE = -11,
    STD_ERROR_HANDLE = -12,
};
const DWORD ATTACH_PARENT_PROCESS = -1;

ENUM(DWORD, CodePage){
    CP_UTF8 = 65001,
};
CASSERT(sizeof(CodePage) == 4);
CASSERT(sizeof(CP_UTF8) == 4);

#pragma comment(lib, "kernel32.lib")
foreign BOOL AttachConsole(DWORD process_id);
foreign HANDLE GetStdHandle(DWORD std_handle);
foreign BOOL SetConsoleOutputCP(CodePage code_page);
foreign BOOL WriteConsoleA(HANDLE console_handle, const byte* buffer, DWORD chars_to_write, DWORD* chars_written, rawptr reserved);
foreign void ExitProcess(CUINT exit_code);

HANDLE stdin, stdout, stderr;
void init_console() {
  AttachConsole(ATTACH_PARENT_PROCESS);
  stdin = GetStdHandle(STD_INPUT_HANDLE);
  stdout = GetStdHandle(STD_OUTPUT_HANDLE);
  stderr = GetStdHandle(STD_ERROR_HANDLE);
}
#elif OS_LINUX
void init_console() {};
#else
CASSERT(false);
#endif
