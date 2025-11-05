#pragma once
#include <stdbool.h>
#include <stdint.h> /* IWYU pragma: keep */

// preprocessor helpers
// #define CONCAT0(a, b) a##b
// #define CONCAT(a, b) CONCAT0(a, b)
#define CONCAT(a, b) a##b
#define STR(a) #a
#define VAR(name, counter) CONCAT(__##name, counter)

#define IF_1(t, f) t
#define IF_0(t, f) f
#define IF(cond, t, f) CONCAT(IF_, cond)(t, f)

#define PROBE() 1, 1
#define SECOND(a, b, ...) b
/* NOTE: SECOND() is also acting like EXPAND() here... */
#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)
#define IS_STRING_String PROBE()
#define IS_STRING(x) IS_PROBE(CONCAT(IS_STRING_, x))

// keywords
/* private to file */
#define readonly const
#define private static
#define global static
#define foreign __declspec(dllimport)
#define naked __attribute__((naked))
#define noreturn _Noreturn void
// #define deprecated(msg) __attribute__((deprecated(msg)))

#define ASSERT(condition) _Static_assert((condition), #condition);
#define DISTINCT(type, name) \
  typedef type name;
#define ENUM(type, name) \
  typedef type name;     \
  enum name : type

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

// typedef __bf16 bf16;
// ASSERT(sizeof(bf16) == 2);
// typedef _Float16 f16;
// ASSERT(sizeof(f16) == 2);
typedef float f32;
ASSERT(sizeof(f32) == 4);
typedef double f64;
ASSERT(sizeof(f64) == 8);

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
  void* ptr;
  intptr count;
} Slice;
/* NOTE: utf8 string */
typedef struct {
  byte* ptr;
  intptr size;
} String;
/* NOTE: clang on linux is *way* too aggressive with freeing unused variables,
    including variables in the outer scope that it shouldn't be touching */
#if 1
  #define string(const_cstr) string_impl(__COUNTER__, const_cstr)
  #define string_impl(counter, const_cstr) ({                                    \
    const byte CONCAT(_cstr, counter)[] = const_cstr;                            \
    (String){(byte*)CONCAT(_cstr, counter), sizeof(CONCAT(_cstr, counter)) - 1}; \
  })
#else
  #define string(const_cstr) ((String){(byte*)const_cstr, sizeof(const_cstr) - 1})
#endif
String str_slice(String str, intptr i, intptr j) {
  String sliced = (String){&str.ptr[i], j - i};
  if (i > j) {
    sliced.size = 0;
  }
  return sliced;
}

// OS_xxx
#define OS_WINDOWS (_WIN32 || _WIN64)
#define OS_LINUX (__linux__ || __unix__)

// ARCH_xxx
#define ARCH_X64 __x86_64__
#define ARCH_X86 __i386__
#define ARCH_ARM64 __aarch64__
#define ARCH_ARM32 (__arm__ && !ARCH_ARM64)

#define ARCH_IS_64BIT (ARCH_X64 || ARCH_ARM64)
#define ARCH_IS_32BIT (ARCH_X86 || ARCH_ARM32)
/* NOTE: stack grows downwards on almost all architectures */
#define ARCH_STACK_DIRECTION (-1)

/* NOTE: windows starts aligned to 8B, while linux starts (correctly) aligned to 16B
  thus we have to realign ourselves either way... */
#if ARCH_X64
  #define ALIGN_STACK_POINTER() asm volatile("andq $-16, %%rsp" ::: "rsp");
  #define CALL(name) asm volatile("call " #name)
#endif

// atomics: https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
#define volatile_store(address, value) __atomic_store_n(address, value, __ATOMIC_RELAXED)
#define volatile_load(address, value) __atomic_load_n(address, __ATOMIC_RELAXED)
#define compiler_fence() __atomic_signal_fence(__ATOMIC_SEQ_CST)
#define mfence() __atomic_thread_fence(__ATOMIC_SEQ_CST)

#define atomic_store(address, value) __atomic_store_n(address, value, __ATOMIC_SEQ_CST)
#define atomic_load(address) __atomic_load_n(address, __ATOMIC_SEQ_CST)
#define atomic_exchange(address, value) __atomic_exchange_n(address, value, __ATOMIC_SEQ_CST)
#define atomic_compare_exchange(address, expected, value) __atomic_compare_exchange(address, expected, value, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_compare_exchange_weak(address, expected, value) __atomic_compare_exchange(address, expected, value, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_add(address, value) __atomic_fetch_add(address, value, __ATOMIC_SEQ_CST)
#define atomic_sub(address, value) __atomic_fetch_sub(address, value, __ATOMIC_SEQ_CST)
#define atomic_and(address, value) __atomic_fetch_and(address, value, __ATOMIC_SEQ_CST)
#define atomic_or(address, value) __atomic_fetch_or(address, value, __ATOMIC_SEQ_CST)
#define atomic_xor(address, value) __atomic_fetch_xor(address, value, __ATOMIC_SEQ_CST)
#define atomic_nand(address, value) __atomic_fetch_nand(address, value, __ATOMIC_SEQ_CST)
ASSERT(__atomic_always_lock_free(1, 0))
ASSERT(__atomic_always_lock_free(2, 0))
ASSERT(__atomic_always_lock_free(4, 0))
ASSERT(__atomic_always_lock_free(8, 0))

#define cpu_relax() asm volatile("pause")
