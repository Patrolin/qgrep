// odin run qgrep -default-to-nil-allocator -no-entry-point
package main
import "base:intrinsics"
import "core:fmt"
import "lib"

main :: proc() {
	file_walk := lib.make_file_walk()
	lib.run_multicore(main_multicore)
}
main_multicore :: proc() {
	// parse args
	options: ^QGrepOptions = ---
	if lib.sync_is_first() {
		options = parse_args(allocator = context.allocator)
	}
	lib.barrier(&options)

	for {
		// read input until we get a valid pattern
		pattern: ^lib.ASTNode = ---
		if lib.sync_is_first() {
			pattern = read_and_parse_console_input_until_valid_pattern(options.input_prompt)
			if options.debug {
				lib.print_ast(pattern)
				continue
			}
		}
		lib.barrier(&pattern)

		// print lines filtered by the pattern
		qgrep_multicore(options, pattern)
	}
}
qgrep_multicore :: proc(options: ^QGrepOptions, pattern: ^lib.ASTNode) {
	// make a shared list of files
	file_walk: ^lib.FileWalk = ---
	if lib.sync_is_first() {
		file_walk = lib.make_file_walk()
	}
	lib.barrier(&file_walk)

	// asynchronously compute the list of files
	if lib.sync_is_first() {
		dir_path := options.path_prefix != "" ? options.path_prefix : "."
		lib.walk_files(dir_path, file_walk)
	}
	// asynchronously walk the list of files
	for {
		current_index := intrinsics.atomic_load(&file_walk.current_index)
		ok: bool
		for current_index < len(file_walk.file_paths) {
			current_index, ok = intrinsics.atomic_compare_exchange_strong(&file_walk.current_index, current_index, current_index + 1)
			if ok {
				file_path := file_walk.file_paths[current_index]
				// default filter `not path ("/." then "/")`
				if !options.include_dot_dirs {
					i := lib.index(file_path, 0, "/.")
					j := lib.index(file_path, i, "/")
					if j != len(file_path) {continue}
				}
				// filter path
				/* TODO: filter path by user input */
				// filter line
				/* TODO: read the file and filter lines by user input */
				fmt.printfln("thread %v: %v", context.user_index, file_path)
			}
		}
		if intrinsics.atomic_load(&file_walk.have_all_file_paths) {break}
	}
	lib.barrier()
}
