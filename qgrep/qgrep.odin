// odin run qgrep -default-to-nil-allocator
// odin build qgrep -default-to-nil-allocator -o:speed
package main
import "../lib"
import "base:intrinsics"
import "core:fmt"

FileWalk :: struct {
	file_paths:           [dynamic]string,
	have_all_file_paths:  bool,
	current_index:        int,
	include_node_modules: bool,
	include_dot_dirs:     bool,
}

main :: proc() {
	/* TODO: run multicore, write to a log and sort the log after */
	lib.run_multicore(main_multicore, 1)
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
			pattern = read_and_parse_console_input_until_valid_pattern(options)
			if options.debug {lib.print_ast(pattern)}
		}
		lib.barrier(&pattern)

		// make a shared list of files
		file_walk: ^FileWalk = ---
		if lib.sync_is_first() {
			file_walk := new(FileWalk, allocator = context.temp_allocator)
			file_walk.file_paths = {}
			file_walk.file_paths.allocator = context.temp_allocator
			file_walk.include_node_modules = options.include_node_modules
			file_walk.include_dot_dirs = options.include_dot_dirs
		}
		lib.barrier(&file_walk)

		fmt.printfln("f.0: %p", file_walk)
		// asynchronously compute the list of files
		if lib.sync_is_first() {
			dir_path := options.path_prefix != "" ? options.path_prefix : "."
			lib.walk_paths(dir_path, file_walk, proc(path: string, user_data: rawptr, is_dir: bool) -> (keep_going: bool) {
				fmt.printfln("walk_proc: '%v', %p, %v", path, user_data, is_dir)
				fw := (^FileWalk)(user_data)
				if is_dir {
					fmt.printfln("ayaya.0")
					fmt.printfln("ayaya.1: %x", fw.current_index)
					if !fw.include_node_modules && lib.ends_with(path, "node_modules") {return false}
					fmt.printfln("ayaya.2")
					if !fw.include_dot_dirs {
						i := lib.index(path, 0, "/.")
						j := lib.index(path, i, "/")
						fmt.printfln("ayaya.3")
						return j != len(path)
					}
				} else {
					BINARY_SUFFIXES :: []string{".pyc", ".pdb", ".bin", ".exe", ".out"}
					for suffix in BINARY_SUFFIXES {
						if lib.ends_with(path, suffix) {return is_dir}
					}
					file_paths := &fw.file_paths
					append(file_paths, path)
				}
				return is_dir
			})
			fmt.printfln("foo")
			intrinsics.atomic_store(&file_walk.have_all_file_paths, true)
			fmt.printfln("foo.1")
			fmt.printfln("wa: %v", file_walk.file_paths[:4])
		}
		// asynchronously print lines filtered by the pattern
		qgrep_multicore(options, pattern, file_walk)
	}
}
qgrep_multicore :: proc(options: ^QGrepOptions, pattern: ^lib.ASTNode, file_walk: ^FileWalk) {
	context.temp_allocator = lib.sub_arena()
	found_matching_path := false
	for {
		current_index := intrinsics.atomic_load(&file_walk.current_index)
		ok: bool
		for current_index < len(file_walk.file_paths) {
			current_index, ok = intrinsics.atomic_compare_exchange_strong(&file_walk.current_index, current_index, current_index + 1)
			if ok {
				free_all(context.temp_allocator)
				info: FilterInfo
				info.file_path = file_walk.file_paths[current_index]
				// filter path
				_, found := filter_by_pattern(&info, pattern, 0, .String)
				if found == 0 {continue}
				found_matching_path = true
				// filter line
				file := lib.open_file_for_reading(info.file_path)
				fmt.assertf(file != lib.FileHandle(lib.INVALID_HANDLE), "failed to open file")
				buffer: [4096]byte = ---
				read_start := 0
				bytes_read := 1
				for bytes_read != 0 {
					if bytes_read != 0 {
						bytes_read = lib.read_from_file(file, buffer[read_start:])
					}
					slice := transmute(string)(buffer[:read_start + bytes_read])
					for len(slice) >= 512 || (len(slice) > 0 && bytes_read == 0) {
						i := lib.index_newline(slice, 0)
						info.line_number += 1
						info.line = slice[:i]
						//fmt.printfln("-- '%v'", info.line)
						_, found := filter_by_pattern(&info, pattern, 0, .String)
						if found != 0 {
							if options.webstorm_compatibility {
								fmt.printfln("%v\nat %v:%v", info.line, info.file_path, info.line_number)
							} else {
								fmt.printfln("%v:%v %v", info.file_path, info.line_number, info.line)
							}
						}
						j := lib.index_ignore_newline(slice, i)
						slice = slice[j:]
					}
					lib.copy(transmute([]byte)slice, buffer[:])
					read_start = len(slice)
				}
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
filter_by_pattern :: proc(info: ^FilterInfo, node: ^lib.ASTNode, start: int, filter_type: FilterType) -> (end: int, found: int) {
	#partial switch TokenType(node.type) {
	case:
		fmt.assertf(false, "Invalid token type: %v", TokenType(node.type))
	case .ParsedInt:
		fmt.assertf(filter_type == .Line, "Misplaced integer: %v", node.slice)
		if info.line_number != 0 {
			found = info.line_number == node.int ? 1 : 0
		} else {
			found = -1
		}
	case .ParsedString:
		fmt.assertf(node.str != "", "Failed to parse string")
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
		case .Line:
			fmt.assertf(false, "Misplaced string: %v", node.slice)
		case .String:
			{
				if info.line_number == 0 {
					found = -1
					break
				}
				found_index := lib.index(info.line, start, node.str)
				if found_index < len(info.line) {
					end = found_index + len(node.str)
					found = 1
				} else {
					end = start
					found = 0
				}
			}
		}
	// unary ops
	case .File:
		end, found = filter_by_pattern(info, node.value, start, .File)
	case .Line:
		end, found = filter_by_pattern(info, node.value, start, .Line)
	case .Not:
		end, found = filter_by_pattern(info, node.value, start, filter_type)
		found = found >= 0 ? 1 - found : found
	// binary ops
	case .And:
		left_end, left_found := filter_by_pattern(info, node.left, start, filter_type)
		right_end, right_found := filter_by_pattern(info, node.right, start, filter_type)
		end = max(left_end, right_end)
		if right_found < 0 {
			found = left_found
		} else if left_found < 0 {
			found = right_found
		} else {
			found = min(left_found, right_found)
		}
	case .Or:
		left_end, left_found := filter_by_pattern(info, node.left, start, filter_type)
		right_end, right_found := filter_by_pattern(info, node.right, start, filter_type)
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
		left_end, left_found := filter_by_pattern(info, node.left, start, filter_type)
		right_end, right_found := filter_by_pattern(info, node.right, left_end, filter_type)
		/* TODO: report error */
		fmt.assertf(left_found >= 0 && right_found >= 0 || left_found == right_found, "Invalid then statement")
		end = right_end
		found = min(left_found, right_found)
	// simplified ops
	case .IndexMulti:
		/* TODO: parse dynamic array and put it in `node.user_data` */
		fmt.assertf(false, "TODO: index_multi()")
	}
	return
}
