#include <stdbool.h>
#include <stdint.h>

// types
typedef unsigned char byte;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/* NOTE: 16b or 32b depending on architecture */
typedef int CINT;
typedef unsigned int CUINT;

typedef intptr_t intptr;
typedef uintptr_t uintptr;
typedef void* rawptr;

/* private to file */
#define private static
#define global static
#define foreign __declspec(dllimport)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define LIB(library_name) __pragma(comment(lib, TOSTRING(library_name)))

// OS_xxx
#define OS_WINDOWS (_WIN32 || _WIN64)
#define OS_LINUX (__linux__ || __unix__)

// ARCH_xxx
#define ARCH_X64 __x86_64__ || _M_X64
#define ARCH_X86 __i386__ || _M_IX86
#define ARCH_ARM64 __aarch64__
