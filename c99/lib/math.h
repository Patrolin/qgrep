#pragma once
#include "definitions.h"
#include "fmt.h"  // IWYU pragma: keep

#define downcast(t1, v1, t2) ({ \
  t2 value_t2 = (t2)v1;         \
  assert((t1)(value_t2) == v1); \
  value_t2;                     \
})
#define saturate(t1, v1, t2) ((t2)(min(value, CONCAT(t2, (t1)_MAX))))
