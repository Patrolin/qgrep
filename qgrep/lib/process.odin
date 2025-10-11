package lib
import "core:fmt"
import "core:os"

@(require_results)
get_args :: proc(allocator := context.temp_allocator) -> (args: ^[dynamic]string) {
	args = new([dynamic]string, allocator = allocator)
	args.allocator = allocator
	when ODIN_OS == .Windows {
		wargs := GetCommandLineW()
		i := 0
		for wargs[i] != 0 && (wargs[i] == ' ' || wargs[i] == '\t') {
			i += 1
		}
		for wargs[i] != 0 {
			end_char := u16(0)
			if wargs[i] == '"' {
				end_char = '"'
				i += 1
			} else if wargs[i] == '\'' {
				end_char = '\''
				i += 1
			}
			j := i
			for wargs[j] != 0 && (wargs[j] != end_char) {
				j += 1
			}
			append(args, copy_cwstr_to_string(&wargs[i], j - i, allocator = allocator))

			i = j
			if end_char != 0 && wargs[j] == end_char {
				i += 1
			}
			for wargs[i] != 0 && (wargs[i] == ' ' || wargs[i] == '\t') {
				i += 1
			}
		}
	} else when ODIN_OS == .Linux {
		args_file, ok := read_file("/proc/self/cmdline", allocator = allocator)
		fmt.assertf(ok, "Failed to read command line arguments")
		i := 0
		for c, j in args_file {
			if c == 0 {
				append(args, args_file[i:j])
				i = j + 1
			}
		}
	} else {
		assert(false)
	}
	return
}
read_console_input :: proc(allocator := context.temp_allocator) -> (input: ^string) {
	input = new(string, allocator = allocator)
	sb := string_builder(allocator = allocator)
	when ODIN_OS == .Windows {
		for {
			wcbuffer: [2048]u16 = ---
			wchars_read: DWORD = ---
			ok := bool(ReadConsoleW(Handle(os.stdin), &wcbuffer[0], len(wcbuffer), &wchars_read, nil))
			fmt.assertf(ok, "Failed to read console input, err: %v", GetLastError())
			wstr := wcbuffer[:wchars_read]
			str := copy_cwstr_to_string(&wstr[0], len(wstr))
			fmt.sbprint(&sb, str)
			if ends_with(str, "\n") {
				offset := ends_with(str, "\r\n") ? 2 : 1
				acc := to_string(sb)
				input^ = acc[:len(acc) - offset]
			}
		}
	} else {
		assert(false)
	}
	return
}
exit_process :: proc(exit_code: u32 = 0) {
	when ODIN_OS == .Windows {
		ExitProcess(exit_code)
	} else when ODIN_OS == .Linux {
		exit(i32(exit_code))
	} else {
		assert(false)
	}
}
