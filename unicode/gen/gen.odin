package gen_unicode
import "../../lib"
import "core:fmt"
import "core:strconv"

// https://www.unicode.org/reports/tr44/#UnicodeData.txt
// first column is implicit
UnicodeData :: struct {
	r:              rune,
	Name:           string,
	class:          struct {
		// If `Mn`, `Mc` or `Me`, then combining
		General_Category:          string,
		// May or may not be zero...
		Canonical_Combining_Class: int,
		Bidi_Class:                string,
	},
	/* https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
	   If `<xxx>yyyy zzzz` then NFKD.
		 If `yyyy zzzz`, then NFD.
	*/
	decomposition:  struct {
		type:    string,
		mapping: string,
	},
	numeric_type:   struct {
		Numeric_Type_Decimal: string,
		Numeric_Type_Digit:   string,
		Numeric_Type_Numeric: string,
	},
	Bidi_Mirrored:  string,
	Unicode_1_Name: string,
	Comment:        string,
	case_mapping:   struct {
		Simple_Uppercase_Mapping: string,
		Simple_Lowercase_Mapping: string,
		Simple_Titlecase_Mapping: string,
	},
}
read_file :: proc(file_path: string) -> string {
	text, ok := lib.read_file(file_path)
	fmt.assertf(ok, "Failed to read file '%v'", file_path)
	return text
}
main :: proc() {
	lib.init_console()

	RuneRange :: struct {
		start, end: i32 `fmt:"#X"`,
	}
	UnicodeInfo :: struct {
		combining_ranges:    [dynamic]RuneRange `fmt:"-"`,
		combining_class:     map[rune]int `fmt:"-"`,
		nfd_decompositions:  map[rune]string `fmt:"-"`,
		nfkd_decompositions: map[rune]string `fmt:"-"`,
	}
	info: UnicodeInfo

	parse_file(
		"unicode/gen/UnicodeData.txt",
		&info,
		proc(user_data: rawptr, line: string) {
			info := (^UnicodeInfo)(user_data)

			parser := CSVParser{line, 0, ";"}
			row: UnicodeData
			r_str := next_column(&parser)
			r_uint, r_ok := strconv.parse_uint(r_str, 16)
			fmt.assertf(r_ok, "Failed to parse rune")
			row.r = rune(u32(r_uint))
			row.Name = next_column(&parser)
			row.class.General_Category = next_column(&parser)
			Canonical_Combining_Class := next_column(&parser)
			ccc, ccc_ok := strconv.parse_int(Canonical_Combining_Class)
			fmt.assertf(ccc_ok, "Failed to parse Canonical_Combining_Class")
			row.class.Canonical_Combining_Class = ccc
			row.class.Bidi_Class = next_column(&parser)
			Decomposition_Type_And_Mapping := next_column(&parser)
			if lib.starts_with(Decomposition_Type_And_Mapping, "<") {
				i := lib.index(Decomposition_Type_And_Mapping, 0, "> ")
				row.decomposition.type = Decomposition_Type_And_Mapping[:i]
				row.decomposition.mapping = Decomposition_Type_And_Mapping[i + 2:]
				fmt.assertf(row.decomposition.mapping != "", "Failed to parse NFKD decomposition")
			} else {
				row.decomposition.mapping = Decomposition_Type_And_Mapping
			}
			fmt.assertf(row.decomposition.mapping != r_str, "Why would this exist?")
			row.numeric_type.Numeric_Type_Decimal = next_column(&parser)
			row.numeric_type.Numeric_Type_Digit = next_column(&parser)
			row.numeric_type.Numeric_Type_Numeric = next_column(&parser)
			row.Bidi_Mirrored = next_column(&parser)
			row.Unicode_1_Name = next_column(&parser)
			row.Comment = next_column(&parser)
			row.case_mapping.Simple_Uppercase_Mapping = next_column(&parser)
			row.case_mapping.Simple_Lowercase_Mapping = next_column(&parser)
			row.case_mapping.Simple_Titlecase_Mapping = next_column_or_empty(&parser)
			fmt.assertf(parser.start == len(line), "Failed to parse entire line: '%v'", line[parser.start:])

			// is_combining
			is_combining := row.class.General_Category == "Mn" || row.class.General_Category == "Mc" || row.class.General_Category == "Me"
			if is_combining {
				last_range: ^RuneRange
				if len(info.combining_ranges) > 0 {
					last_range = &info.combining_ranges[len(info.combining_ranges) - 1]
				}
				if len(info.combining_ranges) > 0 && last_range.end == i32(row.r) - 1 {
					last_range.end += 1
				} else {
					append(&info.combining_ranges, RuneRange{i32(row.r), i32(row.r)})
				}
			}
			// combining_class
			info.combining_class[row.r] = row.class.Canonical_Combining_Class
			// decomposition
			if row.decomposition.mapping != "" {
				if row.decomposition.type == "" {
					info.nfd_decompositions[row.r] = row.decomposition.mapping
				}
				info.nfkd_decompositions[row.r] = row.decomposition.mapping
			}
		},
	)
	/* TODO: compositions */
	for k, v in info.nfd_decompositions {
		i := lib.index_ascii_char(v, 0, ' ')
		first_char_int, ok := strconv.parse_u64_of_base(v[:i], 16)
		fmt.assertf(ok, "Failed to parse first_char: '%v'", v)
		first_char := rune(u32(first_char_int))
		is_composition := (len(v) == 2 * 5 - 1)
		is_composition &&= info.combining_class[k] == 0
		is_composition &&= info.combining_class[first_char] == 0
		//is_composition &&= !in_exclusions_file()
		if is_composition {
			fmt.printfln("%v, %#X, %v", k, k, v)
		}
	}

	// print sizes
	zero_combining_class_size := 0
	nonzero_combining_class_size := 0
	for _, v in info.combining_class {
		if v == 0 {
			zero_combining_class_size += 1
		} else {
			nonzero_combining_class_size += 1
		}
	}
	fmt.printfln("zero_combining_class_size: %v", zero_combining_class_size)
	fmt.printfln("nonzero_combining_class_size: %v", nonzero_combining_class_size)

	combining_ranges_size := 0
	for range in info.combining_ranges {
		combining_ranges_size += int(range.end) - int(range.start) + 1
	}
	fmt.printfln("combining_ranges_count: %v", len(info.combining_ranges))
	fmt.printfln("combining_ranges_size: %v", combining_ranges_size)
	fmt.printfln("nfd_decompositions: %v", len(info.nfd_decompositions))
	fmt.printfln("nfkd_decompositions: %v", len(info.nfkd_decompositions))

	// generate header
	file := lib.open_file_for_writing_and_truncate("unicode/unicode_generated.odin")
	fmt.assertf(file != lib.FileHandle(lib.INVALID_HANDLE), "Failed to open file for writing")
	lib.write_to_file(file, "// generated by `odin run unicode/gen`, do NOT edit it manually\n")
	lib.write_to_file(file, "package lib_unicode\n")
	lib.write_to_file(file, "import \"../lib\"\n")
	lib.write_to_file(file, "import \"core:fmt\"\n")
	// generate is_combining()
	lib.write_to_file(file, "\n")
	lib.write_to_file(file, "is_combining :: proc(r: rune) -> bool {\n")
	lib.write_to_file(file, "  switch r {\n")
	lib.write_to_file(file, "  case ")
	for range, i in info.combining_ranges {
		comma_str := i != 0 ? ", " : ""
		if range.start == range.end {
			lib.write_to_file(file, fmt.tprintf("%v%#X", comma_str, range.start))
		} else {
			lib.write_to_file(file, fmt.tprintf("%v%#X..=%#X", comma_str, range.start, range.end))
		}
	}
	lib.write_to_file(file, ":\n")
	lib.write_to_file(file, "    return true\n")
	lib.write_to_file(file, "  case:\n")
	lib.write_to_file(file, "    return false\n")
	lib.write_to_file(file, "  }\n")
	lib.write_to_file(file, "}\n")
	// generate normalize_nfd(), normalize_nfkd()
	generate_normalize :: proc(file: lib.FileHandle, proc_name: string, decompositions: map[rune]string) {
		lib.write_to_file(file, "\n")
		max_chars := 0
		for _, v in decompositions {
			chars := (len(v) + 1) / 5
			max_chars = max(chars, max_chars)
		}
		lib.write_to_file(file, fmt.tprintf("/* Decompose characters into 1-%v characters */\n", max_chars))
		lib.write_to_file(file, proc_name)
		lib.write_to_file(file, " :: proc(str: string, allocator := context.temp_allocator) -> string {\n")
		lib.write_to_file(file, "  sb := lib.string_builder(allocator = allocator)\n")
		lib.write_to_file(file, "  for r in str {\n")
		lib.write_to_file(file, "    switch r {\n")
		for k in lib.sort_keys(decompositions) {
			v := decompositions[k]
			lib.write_to_file(file, fmt.tprintf("    case %#X:\n", k))
			parser := CSVParser{v, 0, " "}
			for {
				next_value := next_column_or_empty(&parser)
				if next_value == "" {break}
				next_value_int, ok := strconv.parse_uint(next_value, 16)
				fmt.assertf(ok, "Failed to parse next_value")
				lib.write_to_file(file, fmt.tprintf("      fmt.sbprint(&sb, rune(%#X))\n", next_value_int))
			}
		}
		lib.write_to_file(file, "    case:\n")
		lib.write_to_file(file, "      fmt.sbprint(&sb, r)\n")
		lib.write_to_file(file, "    }\n")
		lib.write_to_file(file, "  }\n")
		lib.write_to_file(file, "  return lib.to_string(sb)\n")
		lib.write_to_file(file, "}\n")
	}
	generate_normalize(file, "normalize_nfd", info.nfd_decompositions)
	generate_normalize(file, "normalize_nfkd", info.nfkd_decompositions)
	/* TODO: generate (normalize_nfd(), ...), (to_lower(), ...) */
}

CSVParser :: struct {
	line:      string,
	start:     int,
	separator: string,
}
next_column_or_empty :: proc(parser: ^CSVParser) -> (column: string) {
	i := lib.index(parser.line, parser.start, parser.separator)
	column = parser.line[parser.start:i]
	parser.start = min(i + len(parser.separator), len(parser.line))
	return
}
next_column :: proc(parser: ^CSVParser, loc := #caller_location) -> (column: string) {
	start := parser.start
	column = next_column_or_empty(parser)
	fmt.assertf(parser.start != start, "Invalid column: '%v'", parser.line[start:], loc = loc)
	return
}

ParseProc :: proc(user_data: rawptr, line: string)
parse_file :: proc(src_path: string, user_data: rawptr, parse_proc: ParseProc) {
	src_file := read_file(src_path)
	i := 0
	for i < len(src_file) {
		uncommented_end := lib.index_ascii(src_file, i, "#\r\n")
		line := src_file[i:uncommented_end]
		line = lib.trim_suffix(line, " ")
		if len(line) > 0 {parse_proc(user_data, line)}
		i = lib.index_newline(src_file, uncommented_end)
		i = lib.index_ignore_newline(src_file, i)
	}
}
