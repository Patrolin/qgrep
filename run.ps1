rm foo.rdi -ErrorAction SilentlyContinue;
rm foo.pdb -ErrorAction SilentlyContinue;
clang c99/main.c -std=gnu99 -nostdlib -mno-stack-arg-probe -Werror -Wconversion -O2 -flto -g -fuse-ld=lld -o foo.exe && foo.exe
