#pragma once
#include "definitions.h"
#include "fmt_float.h"

// nan
#if !HAS_CRT
bool isinf(f64 f) {
  u64 x = bitcast(f, f64, u64);
  u64 exponent = x & (u64(0x7ff) << EXPLICIT_MANTISSA_BITS_f64);
  u64 mantissa = x & ((u64(1) << EXPLICIT_MANTISSA_BITS_f64) - 1);
  return exponent == 0x7ff && mantissa == 0;
}
bool isnan(f64 f) {
  u64 x = bitcast(f, f64, u64);
  u64 exponent = x & (u64(0x7ff) << EXPLICIT_MANTISSA_BITS_f64);
  u64 mantissa = x & ((u64(1) << EXPLICIT_MANTISSA_BITS_f64) - 1);
  return exponent == 0x7ff && mantissa != 0;
}
f64 NAN = 0.0 / 0.0;
#endif

// negatives
#if HAS_CRT
  #include <math.h>
#else
  #define abs(v) abs_impl(__COUNTER__, typeof(v), v)
  #define abs_impl(C, t, v) ({                                \
    t VAR(value, C) = v;                                      \
    (VAR(value, C) >= (t)0) ? VAR(value, C) : -VAR(value, C); \
  })
  #define labs(v) abs(v)
  #define llabs(v) abs(v)
  #define fabsf(v) abs(v)
  #define fabs(v) abs(v)
  #define min(v1, v2) min_impl(__COUNTER, typeof(v1), v1, v2)
  #define min_impl(C, t, v1, v2) ({                                 \
    t VAR(left, C) = v1;                                            \
    t VAR(right, C) = v2;                                           \
    (VAR(left, C) <= VAR(right, C)) ? VAR(left, C) : VAR(right, C); \
  })
  #define max(t, v1, v2) max_impl(__COUNTER, t, v1, v2)
  #define max_impl(C, t, v1, v2) ({                                 \
    t VAR(left, C) = v1;                                            \
    t VAR(right, C) = v2;                                           \
    (VAR(left, C) >= VAR(right, C)) ? VAR(left, C) : VAR(right, C); \
  })
#endif

// casts
#define downcast(t1, v, t2) downcast_impl(__COUNTER__, t1, v, t2)
#define downcast_impl(C, t1, v, t2) ({    \
  ASSERT(sizeof(t2) < sizeof(t1));        \
  t1 VAR(v1, C) = v;                      \
  t2 VAR(v2, C) = (t2)VAR(v1, C);         \
  assert((t1)(VAR(v2, C)) == VAR(v1, C)); \
  VAR(v2, C);                             \
})
#define saturate(t1, v1, t2) ({    \
  ASSERT(sizeof(t2) < sizeof(t1)); \
  (t2)(min(t1, v1, (t1)MAX(t2)));  \
})

// floats
#define SPLIT_FLOAT_IMPL(R, F, U, x, mask, shift, bias) \
  ASSERT(sizeof(x) == sizeof(mask));                    \
  ASSERT(sizeof(x) == sizeof(shift));                   \
  ASSERT(sizeof(x) == sizeof(bias));                    \
  bool negate = x < 0;                                  \
  x = negate ? -x : x;                                  \
  if (expect_unlikely(x < 1)) {                         \
    return (R){0, negate ? -x : x};                     \
  };                                                    \
                                                        \
  U y = bitcast(x, F, U);                               \
  U exponent = (y >> shift) & mask - bias;              \
                                                        \
  if (expect_small(exponent < shift)) {                 \
    y &= ~((1 << (shift - exponent)) - 1);              \
  }                                                     \
  F integer = bitcast(y, U, F);                         \
  F fraction = x - integer;                             \
  return (R){negate ? -integer : integer, negate ? -fraction : fraction};

typedef struct {
  f64 integer, fraction;
} SplitFloat_f64;
SplitFloat_f64 split_float_f64(f64 x) {
  u64 exponent_bits = 11;
  u64 mask = (1 << exponent_bits) - 1;
  u64 shift = sizeof(f64) - exponent_bits - 1;
  u64 bias = mask >> 1;
  SPLIT_FLOAT_IMPL(SplitFloat_f64, f64, u64, x, mask, shift, bias)
}
typedef struct {
  f32 integer, fraction;
} SplitFloat_f32;
SplitFloat_f32 split_float_f32(f32 x) {
  u32 exponent_bits = 8;
  u32 mask = (1 << exponent_bits) - 1;
  u32 shift = sizeof(f32) - exponent_bits - 1;
  u32 bias = mask >> 1;
  SPLIT_FLOAT_IMPL(SplitFloat_f32, f32, u32, x, mask, shift, bias)
}
#if ARCH_HAS_NATIVE_BF16
typedef struct {
  bf16 integer, fraction;
} SplitFloat_bf16;
SplitFloat_bf16 split_float_bf16(bf16 x) {
  u16 exponent_bits = 8;
  u16 mask = u16(1 << exponent_bits) - 1;
  u16 shift = sizeof(bf16) - exponent_bits - 1;
  u16 bias = mask >> 1;
  SPLIT_FLOAT_IMPL(SplitFloat_bf16, bf16, u16, x, mask, shift, bias)
}
#endif
#if ARCH_HAS_NATIVE_F16
typedef struct {
  f16 integer, fraction;
} SplitFloat_f16;
SplitFloat_f16 split_float_f16(f16 x) {
  u16 exponent_bits = 5;
  u16 mask = u16(1 << exponent_bits) - 1;
  u16 shift = sizeof(f16) - exponent_bits - 1;
  u16 bias = mask >> 1;
  SPLIT_FLOAT_IMPL(SplitFloat_f16, f16, u16, x, mask, shift, bias)
}
#endif
