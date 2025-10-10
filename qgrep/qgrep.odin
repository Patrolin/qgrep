// odin run qgrep -default-to-nil-allocator -no-entry-point
package qgrep
import "base:intrinsics"
import "core:fmt"
import "lib"

main :: proc() {
	lib.run_multithreaded(main_multithreaded)
}
main_multithreaded :: proc() {
	file_walk: ^lib.FileWalk = ---
	if lib.sync_is_first() {
		file_walk = lib.make_file_walk()
	}
	lib.barrier_sync(&file_walk)

	if lib.sync_is_first() {
		lib.walk_files(".", file_walk)
	}
	for {
		current_index := intrinsics.atomic_load(&file_walk.current_index)
		ok: bool
		for current_index < len(file_walk.file_paths) {
			current_index, ok = intrinsics.atomic_compare_exchange_strong(&file_walk.current_index, current_index, current_index + 1)
			if ok {
				file_path := file_walk.file_paths[current_index]
				/* TODO: read the file and filter by user input */
				fmt.printfln("thread %v: %v", context.user_index, file_path)
			}
		}
		if intrinsics.atomic_load(&file_walk.have_all_file_paths) {break}
	}
	lib.barrier()
}
