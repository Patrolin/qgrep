Format: asm volatile (code : output_variables : input_variables : clobbers)

Code: instruction <input>, <output>

Modifiers
  = - write-only
  + - read-write

Constraints
  r - any register
  m - memory
  g - register or memory
  x - xmm register (also used for floats)
  0 - same as the output
