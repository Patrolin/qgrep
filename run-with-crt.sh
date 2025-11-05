# NOTE: linux has a default stack size limit of 8 MB...
clang c99/main.c -Wno-gcc-compat -std=gnu99 -D HAS_CRT -fno-stack-protector -Werror -Wconversion -O0 -g -o foo-linux-x64 && ./foo-linux-x64
#&& objcopy --only-keep-debug foo-linux-x64 foo-linux-x64.debug
#&& strip --strip-debug foo-linux-x64
#&& objcopy --add-gnu-debuglink=foo-linux-x64.debug foo-linux-x64
