// odin run qgrep -default-to-nil-allocator
package main
import "base:intrinsics"
import "core:fmt"
import "lib"

main :: proc() {
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
			if options.debug {lib.print_ast(pattern)}
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
	found_matching_path := false
	for {
		current_index := intrinsics.atomic_load(&file_walk.current_index)
		ok: bool
		for current_index < len(file_walk.file_paths) {
			current_index, ok = intrinsics.atomic_compare_exchange_strong(&file_walk.current_index, current_index, current_index + 1)
			if ok {
				info: FilterInfo
				info.file_path = file_walk.file_paths[current_index]
				// default filter path
				if !options.include_dot_dirs {
					if default_filter_path(info.file_path) == 0 {continue}
				}
				// filter path
				_, found := filter_path(&info, pattern, 0, .String)
				if found == 0 {continue}
				found_matching_path = true
				// filter line
				/* TODO: read the file and filter lines by user input */
				fmt.printfln("thread %v: %v", context.user_index, info)
			}
		}
		if intrinsics.atomic_load(&file_walk.have_all_file_paths) {break}
	}
	found_matching_path_all, is_first := lib.barrier_gather(found_matching_path)
	// print found_matching_path
	if is_first {
		for v in found_matching_path_all {
			found_matching_path ||= v
		}
		if !found_matching_path {fmt.printfln("No matching paths.")}
	}
	lib.barrier()
}
/* `found`: -1 if undefined, 0 if false, 1 if true */
default_filter_path :: proc(file_path: string) -> (found: int) {
	// not file ("/." then "/")
	i := lib.index(file_path, 0, "/.")
	j := lib.index(file_path, i, "/")
	return j == len(file_path) ? 1 : 0
}
FilterInfo :: struct {
	file_path:   string,
	line_number: int,
	line:        string,
}
FilterType :: enum {
	String,
	File,
	Line,
}
/* `end`: index after the match \
	`found`: -1 if undefined, 0 if false, 1 if true */
filter_path :: proc(info: ^FilterInfo, node: ^lib.ASTNode, start: int, filter_type: FilterType) -> (end: int, found: int) {
	#partial switch TokenType(node.type) {
	case:
		fmt.assertf(false, "Invalid token type: %v", TokenType(node.type))
	case .Number:
		fmt.assertf(filter_type == .Line, "Misplaced integer")
		if info.line_number != 0 {
			assert(false)
		} else {
			found = -1
		}
	case .ParsedString:
		switch filter_type {
		case .File:
			{
				found_index := lib.index(info.file_path, start, node.str)
				if found_index < len(info.file_path) {
					end = found_index + len(node.str)
					found = 1
				} else {
					end = start
					found = 0
				}
			}
		case .Line, .String:
			if info.line_number != 0 {
				assert(false)
			} else {
				found = -1
			}
		}
	// unary ops
	case .Not:
		end, found = filter_path(info, node.value, start, filter_type)
		found = found >= 0 ? 1 - found : found
	case .File:
		end, found = filter_path(info, node.value, start, .File)
	case .Line:
		end, found = filter_path(info, node.value, start, .Line)
	// binary ops
	case .And:
		left_end, left_found := filter_path(info, node.left, start, filter_type)
		right_end, right_found := filter_path(info, node.right, start, filter_type)
		end = max(left_end, right_end)
		if right_found < 0 {
			found = left_found
		} else if left_found < 0 {
			found = right_found
		} else {
			found = min(left_found, right_found)
		}
	case .Or:
		left_end, left_found := filter_path(info, node.left, start, filter_type)
		right_end, right_found := filter_path(info, node.right, start, filter_type)
		found = max(left_found, right_found)
		if right_found < 0 {
			end = left_end
		} else if left_found < 0 {
			end = right_end
		} else {
			end = min(left_end, right_end)
		}
	case .Then:
		/* NOTE: we always need to run both sides in order to check for undefined */
		left_end, left_found := filter_path(info, node.left, start, filter_type)
		right_end, right_found := filter_path(info, node.right, end, filter_type)
		/* TODO: report error */
		fmt.assertf(left_found >= 0 && right_found >= 0, "Invalid then statement")
		end = right_end
		found = min(left_found, right_found)
	// simplified ops
	case .IndexMulti:
		/* TODO: parse dynamic array and put it in `node.user_data` */
		fmt.assertf(false, "TODO: index_multi()")
	}
	return
}
