package main
import "core:fmt"
import "lib"

QGrepOptions :: struct {
	include_dot_dirs:       bool,
	webstorm_compatibility: bool,
	debug:                  bool,
	path_prefix:            string,
	input_prompt:           string,
}

parse_args :: proc(allocator := context.temp_allocator) -> (options: ^QGrepOptions) {
	args := lib.get_args()
	options = new(QGrepOptions, allocator = allocator)
	input_prompt_sb := lib.string_builder(allocator = allocator)
	fmt.sbprint(&input_prompt_sb, "qgrep")
	for i := 1; i < len(args); i += 1 {
		arg := args[i]
		switch arg {
		case "-webstorm":
			options.webstorm_compatibility = true
			fmt.sbprint(&input_prompt_sb, " -webstorm")
		case "-dotdirs":
			options.include_dot_dirs = true
			fmt.sbprint(&input_prompt_sb, " -dotdirs")
		case "-debug":
			options.debug = true
			fmt.sbprint(&input_prompt_sb, " -debug")
		case "-dir":
			i += 1
			fmt.assertf(i < len(args), "Missing value for -path [string]")
			options.path_prefix = args[i]
			fmt.sbprintf(&input_prompt_sb, " -path \"%v\"", options.path_prefix)
		case:
			fmt.printfln("Unknown argument: '%v'", arg)
			fallthrough
		case "help", "-help":
			fmt.println("Usage:")
			fmt.println("  qgrep -path [string]: start searching from this path")
			fmt.println("  qgrep -dotdirs: disable default filter `not path (\"./\" then \"/\")`")
			fmt.println("  qgrep -webstorm: print links in WebStorm-compatible format")
			fmt.println("  qgrep -debug: print the pattern parsed from user input")
			fmt.println("  qgrep -help: print this")
			fmt.println("  qgrep help: print this")
			lib.exit_process(1)
		}
	}
	fmt.sbprint(&input_prompt_sb, ": ")
	options.input_prompt = lib.to_string(input_prompt_sb)
	return
}
