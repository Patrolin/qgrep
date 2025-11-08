#pragma once
#include "definitions.h"

// negatives
#define abs(t, v) abs_impl(__COUNTER__, t, v)
#define abs_impl(c, t, v) ({                                \
  t VAR(value, c) = v;                                      \
  (VAR(value, c) >= (t)0) ? VAR(value, c) : -VAR(value, c); \
})
#define min(t, v1, v2) min_impl(__COUNTER, t, v1, v2)
#define min_impl(c, t, v1, v2) ({                                 \
  t VAR(left, c) = v1;                                            \
  t VAR(right, c) = v2;                                           \
  (VAR(left, c) <= VAR(right, c)) ? VAR(left, c) : VAR(right, c); \
})
#define max(t, v1, v2) max_impl(__COUNTER, t, v1, v2)
#define max_impl(c, t, v1, v2) ({                                 \
  t VAR(left, c) = v1;                                            \
  t VAR(right, c) = v2;                                           \
  (VAR(left, c) >= VAR(right, c)) ? VAR(left, c) : VAR(right, c); \
})

// casts
#define downcast(t1, v, t2) downcast_impl(__COUNTER__, t1, v, t2)
#define downcast_impl(c, t1, v, t2) ({    \
  t1 VAR(v1, c) = v;                      \
  t2 VAR(v2, c) = (t2)VAR(v1, c);         \
  assert((t1)(VAR(v2, c)) == VAR(v1, c)); \
  VAR(v2, c);                             \
})
#define saturate(t1, v1, t2) ((t2)(min(value, (t1)CONCAT(t2, _MAX))))

// bits: https://gcc.gnu.org/onlinedocs/gcc/Bit-Operation-Builtins.html
#define ptr_add(ptr, offset) ((byte*)ptr + offset)
/* AKA log2_floor() */
#define find_first_set(t, v) __builtin_ffsg((t)(v))
#define log2_ceil(t, v) log2_ceil_impl(__COUNTER__, t, v)
#define log2_ceil_impl(c, t, v) ({                       \
  t1 VAR(value, c) = v1;                                 \
  VAR(value, c) <= 1 ? 0 : find_first_set((x - 1) << 1); \
})
#define count_leading_zeros(t, v) __builtin_clzg((t)(v))
#define count_trailing_zeros(t, v) __builtin_ctzg((t)(v))
#define count_leading_redundant_sign_bits(t, v) __builtin_clrsbg((t)(v))
#define count_ones(t, v) __builtin_popcountg((t)(v))
#define count_zeros(t, v) __builtin_popcountg(~(t)(v))
#define count_parity(t, v) __builtin_parityg((t)(v))

// floats
#define SPLIT_FLOAT_IMPL(R, F, U, x, mask, shift, bias) \
  ASSERT(sizeof(x) == sizeof(mask));                    \
  ASSERT(sizeof(x) == sizeof(shift));                   \
  ASSERT(sizeof(x) == sizeof(bias));                    \
  bool negate = x < 0;                                  \
  x = negate ? -x : x;                                  \
  if (x < 1) {                                          \
    return (R){0, negate ? -x : x};                     \
  };                                                    \
                                                        \
  union {                                               \
    F f;                                                \
    U i;                                                \
  } u;                                                  \
  u.f = x;                                              \
  U exponent = (u.i >> shift) & mask - bias;            \
                                                        \
  if (exponent < shift) {                               \
    u.i &= ~((1 << (shift - exponent)) - 1);            \
  }                                                     \
  F integer = u.f;                                      \
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
  u16 mask = (u16)(1 << exponent_bits) - 1;
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
  u16 mask = (u16)(1 << exponent_bits) - 1;
  u16 shift = sizeof(f16) - exponent_bits - 1;
  u16 bias = mask >> 1;
  SPLIT_FLOAT_IMPL(SplitFloat_f16, f16, u16, x, mask, shift, bias)
}
#endif
