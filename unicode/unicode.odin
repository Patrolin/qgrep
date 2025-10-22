package lib_unicode

/*
@(private = "file")
_normalize_compose :: proc(str: string, allocator := context.temp_allocator) -> string {
	/* TODO:
		1. sort nonzero CCC's by CCC ascending
	  2. compose multiple times if:
			a) is a canonical decomposition
			b) decomposes into exactly 2 characters
			c) CCC(character) == 0 && CCC(nfd(character)[0]) == 0
			d) is not in CompositionExclusions.txt
	*/
	assert(false)
}
normalize_nfc :: proc(str: string, allocator := context.temp_allocator) -> string {
	str := normalize_nfd(str)
	return _normalize_compose(str, allocator = allocator)
}
normalize_nfkc :: proc(str: string, allocator := context.temp_allocator) -> string {
	str := normalize_nfkd(str)
	return _normalize_compose(str, allocator = allocator)
}
*/
