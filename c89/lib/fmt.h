
/* TODO: StringBuilder */

void print_u32(u32 value) {
  const char msg[] = "print_u32()\n";
  DWORD bytes_written;
  WriteConsoleA(stdout, msg, sizeof(msg) - 1, &bytes_written, 0);
};
void print_cstr(const byte* cstr) {
  /* TODO: print_cstr */
};
void print_unknown(rawptr ptr) {
  /* TODO: assert */
};

#define print(x) _Generic((x), \
    u32: print_u32,            \
    const char*: print_cstr,   \
    default: print_unknown)(x)
