
#define SYS_WRITE 1
#define SYS_EXIT 60
#define SYS_OPEN 2

#define O_WRONLY 1
void _start(int argc, char** argv) {
  const char* msg = "Hello, world\n";
  const int msg_len = sizeof("Hello, world\n") - 1;
}

void lee(const char msg[]) {
  int fd = 1;
  asm volatile(
      "movq %0, %%rax\n\t"  // SYS_WRITE
      "movq %1, %%rdi\n\t"  // fd
      "movq %2, %%rsi\n\t"  // buffer
      "movq %3, %%rdx\n\t"  // length
      "syscall\n\t"
      :
      : "rax"((long)SYS_WRITE), "g"(fd), "r"(msg), "g"((long)(sizeof(msg) - 1))
      : "rax", "rdi", "rsi", "rdx");

  // Syscall: exit(0)
  asm volatile(
      "movq %0, %%rax\n\t"
      "xor %%rdi, %%rdi\n\t"
      "syscall\n\t"
      :
      : "r"((long)SYS_EXIT)
      : "rax", "rdi");
}
