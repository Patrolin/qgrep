// odin run qgrep -default-to-nil-allocator -no-entry-point
package main
import "base:intrinsics"
import "core:fmt"
import "lib"

QGrepOptions :: struct {
	webstorm_compatibility: bool,
}

main :: proc() {
	lib.run_multicore(main_multicore)
}
main_multicore :: proc() {
	args: ^[dynamic]string = ---
	options: ^QGrepOptions = ---
	if lib.sync_is_first() {
		args = lib.get_args()
		options = new(QGrepOptions, allocator = context.allocator)
		for arg in args[1:] {
			if arg == "webstorm" {
				options.webstorm_compatibility = true
			} else {
				fmt.assertf(false, "Unknown argument: '%v'", arg)
			}
		}
	}
	lib.barrier(&options)

	for {
		free_all(context.temp_allocator)
		if lib.sync_is_first() {
			read_and_parse_input()
		}
		lib.barrier()
	}
}

TokenType :: enum {
	None,
	// whitespace
	Whitespace,
	// values
	String,
	Number,
	// unary ops
	Not,
	File,
	// binary ops
	And,
	Or,
	Then,
}
read_and_parse_input :: proc() {
	fmt.print("qgrep: ")
	input := lib.read_console_input()

	parse_pattern :: proc(parser: ^lib.Parser, prev_token_type: int) -> (token: lib.Token, operator_precedence: int) {
		i := parser.start
		if parser.str[i] == '"' {
			operator_precedence = int(lib.OpType.Value)
			token.type = int(TokenType.String)
			/* TODO: parse strings with backslash escapes */
		}
		j := lib.index_ascii(parser.str, i, " ")
		if j == i {
			token.type = int(TokenType.Whitespace)
			j := i
			for j < len(parser.str) && parser.str[j] == ' ' {j += 1}
			token.slice = parser.str[i:j]
			return token, int(lib.OpType.Ignore)
		} else {
			token.slice = parser.str[i:j]
			switch token.slice {
			case "not":
				token.type = int(TokenType.Not)
				operator_precedence = int(lib.OpType.Unary)
			case "file":
				token.type = int(TokenType.File)
				operator_precedence = int(lib.OpType.Unary)
			case "and":
				token.type = int(TokenType.And)
			case "or":
				token.type = int(TokenType.And)
			case "then":
				token.type = int(TokenType.And)
			case:
				operator_precedence = int(lib.OpType.Value)
				token.type = int(TokenType.Number)
			/* TODO: validate numbers */
			}
			return token, operator_precedence
		}
	}
	pattern := lib.parse(input^, parse_pattern)
	lib.print_ast(pattern)
}
qgrep_multithreaded :: proc(options: ^QGrepOptions, input: ^string) {
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
				// prefilter
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
