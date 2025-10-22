package unicode

@(require_results)
is_combining :: proc(r: rune) -> bool {
	c := i32(r)

	return(
		c >= 0x0300 &&
		(c <= 0x036f || (c >= 0x1ab0 && c <= 0x1aff) || (c >= 0x1dc0 && c <= 0x1dff) || (c >= 0x20d0 && c <= 0x20ff) || (c >= 0xfe20 && c <= 0xfe2f)) \
	)
}

/*
normalize_nfd :: proc(str: string, allocator := context.temp_allocator) -> string {
	return "TODO"
}
normalize_nfkd :: proc(str: string, allocator := context.temp_allocator) -> string {
	return "TODO"
}

@(private = "file")
_normalize_compose :: proc(str: string, allocator := context.temp_allocator) -> string {
	return "TODO"
}
normalize_nfc :: proc(str: string, allocator := context.temp_allocator) -> string {
	str := normalize_nfd(str, allocator = context.temp_allocator)
	return _normalize_compose(str, allocator = allocator)
}
normalize_nfkc :: proc(str: string, allocator := context.temp_allocator) -> string {
	str := normalize_nfkd(str, allocator = context.temp_allocator)
	return _normalize_compose(str, allocator = allocator)
}
*/
