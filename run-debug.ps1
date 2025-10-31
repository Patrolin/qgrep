rm foo.rdi -ErrorAction SilentlyContinue;
rm foo.pdb -ErrorAction SilentlyContinue;
clang c99/main.c -std=gnu99 -nostdlib -mno-stack-arg-probe "-Wl,/STACK:0x100000" -Werror -Wconversion -O0 -g -fuse-ld=lld -o foo.exe && foo.exe
