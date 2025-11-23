param (
    [switch]$opt,
    [switch]$crt
)
$cargs = @("-Wno-gcc-compat", "-march=native", "-masm=intel", "-std=gnu99", "-fno-signed-char")
if ($crt) {
  $cargs += @("-DHAS_CRT") # why does this fail bro...
} else {
  $cargs += @("-nostdlib", "-mno-stack-arg-probe")
}
$cargs += @("-Werror", "-Wconversion", "-Wsign-conversion", "-Wnullable-to-nonnull-conversion")
$cargs += @("-fuse-ld=lld", "-Wl,/STACK:0x100000")
if ($opt) {
  $cargs += @("-O2", "-flto", "-g")
} else {
  $cargs += @("-O0", "-g")
}

rm foo.rdi -ErrorAction SilentlyContinue;
rm foo.pdb -ErrorAction SilentlyContinue;
echo "clang $cargs c99/main.c -o foo.exe && foo.exe"
clang $cargs c99/main.c -o foo.exe && foo.exe
