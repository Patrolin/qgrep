rm foo.rdi -ErrorAction SilentlyContinue;
rm foo.pdb -ErrorAction SilentlyContinue;
clang c99/main.c -Wno-gcc-compat -march=native -masm=intel -std=gnu99 -nostdlib -mno-stack-arg-probe -Werror -Wconversion -O2 -flto -g -fuse-ld=lld "-Wl,/STACK:0x100000" -o foo.exe && foo.exe
