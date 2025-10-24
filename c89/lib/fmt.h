
void print_long_double(CLONGDOUBLE value) {};
void print_double(CDOUBLE value) {};
void print_float(CFLOAT value) {};
void print_CLONGLONG(CLONGLONG value) {};
void print_CULONGLONG(CULONGLONG value) {};
void print_CLONG(CLONG value) {};
void print_CULONG(CULONG value) {};
void print_CINT(CINT value) {};
void print_CUINT(CUINT value) {};
void print_short(CSHORT value) {};
void print_ushort(CUSHORT value) {};
void print_ichar(CICHAR value) {};
void print_uchar(CUCHAR value) {};

void print_u32(u32 value) {
  const char msg[] = "print_u32()\n";
  DWORD bytes_written;
  WriteConsoleA(stdout, msg, sizeof(msg) - 1, &bytes_written, 0);
};
void print_cstr(const byte* cstr) {};
void print_unknown(rawptr ptr) {};

#define print(x) _Generic((x), \
    u32: print_u32,            \
    char*: print_cstr,         \
    const char*: print_cstr,   \
    default: print_unknown)(x)

// long double: print_long_double,             \
// double: print_double,                       \
// float: print_float,                         \
// long long: print_CLONGLONG, /* long long */ \
// unsigned long long: print_CULONGLONG,       \
// long: print_CLONG, /* long */               \
// unsigned long: print_CULONG,                \
// int: print_CINT, /* int */                  \
// unsigned int: print_CUINT,                  \
// short: print_short, /* short */             \
// unsigned short: print_ushort,               \
// char: print_uchar, /* char */               \
// signed char: print_ichar,                   \
// unsigned char: print_uchar,                 \
