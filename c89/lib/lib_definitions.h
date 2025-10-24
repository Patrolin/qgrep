#include <stdbool.h>
#include <stdint.h>

// C standard is dumb
int _fltused;

// distinct
#define CASSERT(Test_For_True) _Static_assert((Test_For_True), #Test_For_True);
#define DISTINCT(Type, Name) \
  typedef struct {           \
    Type value;              \
  } Name;

// enum
#if __clang__
#define ENUM(Type, Name) \
  typedef Type Name;     \
  enum Name : Type
#else
/* NOTE: this has incorrect sizeof(Type), but it's only for intellisense */
#define ENUM(Type, Name) \
  typedef Type Name;     \
  enum Name
#endif

// types
typedef char byte;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
CASSERT(sizeof(f32) == 4);
typedef double f64;
CASSERT(sizeof(f64) == 8);

// typedef signed char CICHAR;
// typedef unsigned char CUCHAR;
// typedef short CSHORT;
// typedef unsigned short CUSHORT;
/* NOTE: 16b or 32b depending on architecture */
typedef int CINT;
typedef unsigned int CUINT;
// typedef long CLONG;
// typedef unsigned long CULONG;
// typedef long long CLONGLONG;
// typedef unsigned long long CULONGLONG;

typedef intptr_t intptr;
typedef uintptr_t uintptr;
typedef void* rawptr;

typedef struct {
  byte* ptr;
  intptr count;
} slice;
DISTINCT(slice, string);

/* private to file */
#define private static
#define global static
#define foreign __declspec(dllimport)

// OS_xxx
#define OS_WINDOWS (_WIN32 || _WIN64)
#define OS_LINUX (__linux__ || __unix__)

// ARCH_xxx
#define ARCH_X64 __x86_64__ || _M_X64
#define ARCH_X86 __i386__ || _M_IX86
#define ARCH_ARM64 __aarch64__
