Format: asm volatile (code : output_variables : input_variables : clobbers)

/* NOTE: we will use intel syntax: -masm=intel
   %0 // first argument (output if present)
   %1 // second argument
   ...
*/
Code: instruction %0, %1, %2

Modifiers
  = - write-only
  + - read-write

Constraints
  r - any register
  m - memory
  g - register or memory
  x - xmm register (also used for floats)
  0 - same as the output, but in a new argument // extremely confusing, do not use
