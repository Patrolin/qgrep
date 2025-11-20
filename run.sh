# NOTE: linux forces a stack size of 8MB...
clang c99/main.c -Wno-gcc-compat -march=native -masm=intel -std=gnu99 -nostdlib -fno-stack-protector -Werror -Wconversion -Wnullable-to-nonnull-conversion -fno-signed-char -O0 -g -o foo-linux-x64 && ./foo-linux-x64
#&& objcopy --only-keep-debug foo-linux-x64 foo-linux-x64.debug
#&& strip --strip-debug foo-linux-x64
#&& objcopy --add-gnu-debuglink=foo-linux-x64.debug foo-linux-x64
