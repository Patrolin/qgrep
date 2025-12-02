param (
    [switch]$opt,
    [switch]$crt,
    [switch]$testFloat
)
$cargs = @("-march=native", "-masm=intel", "-std=gnu99", "-fno-signed-char")
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
$input = "c99/main.c"
$output = "foo"
if ($testFloat) {
  $input = "c99/test_fmt_float.c"
  $output = "test_fmt_float"
}

rm ($output + ".rdi") -ErrorAction SilentlyContinue;
rm ($output + ".pdb") -ErrorAction SilentlyContinue;
echo "clang $cargs $input -o $output.exe && $output.exe"
clang $cargs $input -o ($output + ".exe") && Invoke-Expression ($output + ".exe")
