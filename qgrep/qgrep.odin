// odin run qgrep -default-to-nil-allocator -no-entry-point
package main
import "base:intrinsics"
import "core:fmt"
import "lib"

QGrepOptions :: struct {
	include_dot_paths:      bool,
	webstorm_compatibility: bool,
}

main :: proc() {
	lib.run_multicore(main_multicore, 1)
}
main_multicore :: proc() {
	args: ^[dynamic]string = ---
	options: ^QGrepOptions = ---
	if lib.sync_is_first() {
		args = lib.get_args()
		options = new(QGrepOptions, allocator = context.allocator)
		for arg in args[1:] {
			switch arg {
			case "webstorm":
				options.webstorm_compatibility = true
			case "dotpaths":
				options.include_dot_paths = true
			case:
				fmt.printfln("Unknown argument: '%v'", arg)
				fallthrough
			case "help":
				fmt.println("Usage:")
				fmt.println("  qgrep -webstorm: print links in WebStorm-compatible format")
				fmt.println("  qgrep -dotpaths: disable default filter `not file (\"./\" then \"/\")`")
				lib.exit_process(1)
			}
		}
	}
	lib.barrier(&options)

	for {
		free_all(context.temp_allocator)
		pattern: ^lib.ASTNode = ---
		if lib.sync_is_first() {
			pattern = read_and_parse_console_input()
		}
		lib.barrier(&pattern)

		//qgrep_multicore(options, pattern)
	}
}
qgrep_multicore :: proc(options: ^QGrepOptions, pattern: ^lib.ASTNode) {
	file_walk: ^lib.FileWalk = ---
	if lib.sync_is_first() {
		file_walk = lib.make_file_walk()
	}
	lib.barrier(&file_walk)

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
				// not file ("/." then "/")
				i := lib.index(file_path, 0, "/.")
				j := lib.index(file_path, i, "/")
				if j != len(file_path) {continue}
				// filter

				/* TODO: read the file and filter by user input */
				fmt.printfln("thread %v: %v", context.user_index, file_path)
			}
		}
		if intrinsics.atomic_load(&file_walk.have_all_file_paths) {break}
	}
	lib.barrier()
}
