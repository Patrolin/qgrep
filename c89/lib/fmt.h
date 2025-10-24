
/* TODO: StringBuilder */

#define SBPRINT_SIZE_String(value) value.size
#define SBPRINT_SIZE_uintptr(value) 20

/* TODO: void* stack_realloc(intptr size) */
#define SBPRINT(t1, v1) ({                    \
  intptr offset = 0;                          \
  offset += CONCAT(SBPRINT_SIZE_, t1)(v1);    \
  byte* ptr = (byte*)(uintptr)alloca(offset); \
  byte* ptr_start = ptr;                      \
  offset = 0;                                 \
  offset += CONCAT(sbprint_, t1)(v1, ptr);    \
  (String){ptr_start, offset};                \
});

intptr sbprint_String(String str, byte* buffer) {
  intptr i = 0;
  for (; i < str.size; i++) {
    buffer[i] = str.ptr[i];
  }
  return i;
}
intptr sbprint_uintptr(uintptr value, byte* buffer) {
  intptr size = SBPRINT_SIZE_uintptr(value);
  intptr i = size - 1;
  buffer[i--] = 0;
  while (i >= 0) {
    intptr digit = value % 10;
    value = value / 10;
    buffer[i--] = '0' + digit;
    if (value == 0) {
      break;
    }
  }
  return size - i;
}

void print_String(String str) {
  DWORD bytes_written;
  WriteConsoleA(stdout, str.ptr, str.size, &bytes_written, 0);
}
void print_u32(u32 value) {
  const char msg[] = "print_u32()\n";
  DWORD bytes_written;
  WriteConsoleA(stdout, msg, sizeof(msg) - 1, &bytes_written, 0);
};
void print_unknown(rawptr ptr) {
  /* TODO: assert */
};

#define print(x) _Generic((x), \
    String: print_String,      \
    u32: print_u32,            \
    default: print_unknown)(x)
