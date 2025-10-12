package lib
import "base:intrinsics"
import "core:fmt"
import "core:strings"

/* TODO: when `-target:freestanding_amd64_win64 -no-rtti` is supported, implement non-RTTI fmt */
@(private)
StringBuilder :: strings.Builder

/* NOTE: don't allow uninitialized StringBuilder... */
@(require_results)
string_builder :: #force_inline proc(allocator := context.temp_allocator) -> (sb: StringBuilder) {
	return strings.builder_make_none(allocator)
}
to_string :: #force_inline proc(sb: StringBuilder) -> string {
	return strings.to_string(sb)
}

repeat :: proc(str: string, count: int, allocator := context.temp_allocator) -> string {
	sb := string_builder(allocator = allocator)
	for i in 0 ..< count {
		fmt.sbprint(&sb, str)
	}
	return to_string(sb)
}
