package lib
import "base:intrinsics"
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
