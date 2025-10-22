package lib
import "core:slice"

sort_keys :: proc(items: map[$K]$V) -> []K {
	keys, err := slice.map_keys(items)
	assert_contextless(err == nil)
	slice.sort(keys)
	return keys
}
