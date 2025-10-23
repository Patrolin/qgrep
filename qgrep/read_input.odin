package main
import "../lib"
import "../unicode"
import "core:fmt"
import "core:strconv"

/* TODO: make our own unicode library... */
normalize_string :: proc(input: string, options: ^QGrepOptions, allocator := context.temp_allocator) -> string {
	str := input
	if options.symbol_sensitive {
		str = unicode.normalize_nfd(str)
	} else {
		str = unicode.normalize_nfkd(str)
	}
	assert_contextless(input == "" || str != "")

	if !options.accent_sensitive {
		/* NOTE: `str` must be in NFD or NFKD form at this point */
		sb := lib.string_builder(allocator = allocator)
		for rune in str {
			if !unicode.is_combining(rune) {
				fmt.sbprint(&sb, rune)
			}
		}
		str = lib.to_string(sb)
	}

	if !options.case_sensitive {
		str = lib.lowercase(str)
	}
	return str
}

TokenType :: enum {
	None,
	// ignore
	Whitespace,
	// brackets
	LeftBracket,
	RightBracket,
	// values
	String,
	Int,
	// unary ops
	Not,
	File,
	Line,
	// binary ops
	And,
	Or,
	Then,
	// simplified ops
	ParsedInt = -1,
	ParsedString = -2,
	IndexMulti = -3,
}
read_and_parse_console_input_until_valid_pattern :: proc(options: ^QGrepOptions) -> ^lib.ASTNode {
	for {
		free_all(context.temp_allocator)
		// read console input
		input := lib.read_console_input(options.input_prompt)
		// parse
		parse_pattern :: proc(parser: ^lib.Parser, prev_node: ^lib.ASTNode) -> (token: lib.Token, operator_precedence: int) {
			i := parser.start
			first_char := parser.str[i]
			switch first_char {
			case '(':
				operator_precedence = int(lib.OpType.LeftBracket)
				token.type = int(TokenType.LeftBracket)
				token.slice = parser.str[i:i + 1]
				return
			case ')':
				operator_precedence = int(lib.OpType.RightBracket)
				token.type = int(TokenType.RightBracket)
				token.slice = parser.str[i:i + 1]
				return
			case '"':
				operator_precedence = int(lib.OpType.Value)
				token.type = int(TokenType.String)
				j := lib.index_ascii_char(parser.str, i + 1, '"')
				if j == len(parser.str) {
					error := fmt.tprintf("Unterminated string: %v", parser.str[i:j])
					lib.report_parser_error(parser, error)
				} else {
					token.slice = parser.str[i:j + 1]
				}
				return
			}
			j := lib.index_ascii(parser.str, i, " \"")
			if j == i {
				operator_precedence = int(lib.OpType.Ignore)
				token.type = int(TokenType.Whitespace)
				j := i
				for j < len(parser.str) && parser.str[j] == ' ' {j += 1}
				token.slice = parser.str[i:j]
			} else {
				token.slice = parser.str[i:j]
				switch token.slice {
				case "not":
					operator_precedence = int(lib.OpType.Unary)
					token.type = int(TokenType.Not)
				case "file":
					operator_precedence = int(lib.OpType.Unary)
					token.type = int(TokenType.File)
				case "line":
					operator_precedence = int(lib.OpType.Unary)
					token.type = int(TokenType.Line)
				case "or":
					token.type = int(TokenType.Or)
					operator_precedence = 0
				case "and":
					token.type = int(TokenType.And)
					operator_precedence = 1
				case "then":
					token.type = int(TokenType.Then)
					operator_precedence = 2
				case:
					if prev_node != nil && TokenType(prev_node.type) == .Line {
						operator_precedence = int(lib.OpType.Value)
						token.type = int(TokenType.Int)
						/* TODO: validate numbers */
					} else {
						error := fmt.tprintf("Invalid token: '%v'", token.slice)
						lib.report_parser_error(parser, error)
					}
				}
			}
			return
		}
		pattern, error := lib.parse(input^, parse_pattern)
		if len(error) > 0 {
			fmt.printfln("Error: %v", error)
			continue
		}
		// simplify
		/* TODO: implement IndexMulti */
		//simplify_pattern_first_pass(pattern)
		simplify_pattern_second_pass(pattern, options)
		return pattern
	}
}
simplify_pattern_first_pass :: proc(node: ^lib.ASTNode, is_topmost_or := true) -> (is_index_multi: int) {
	// find places to merge `or`s
	next_is_topmost_or := is_topmost_or && TokenType(node.type) != .Or
	is_index_multi = int(TokenType(node.type) == .Or)
	if node.left != nil {
		is_index_multi &= simplify_pattern_first_pass(node.left, next_is_topmost_or)
	}
	if node.right != nil {
		is_index_multi &= simplify_pattern_first_pass(node.right, next_is_topmost_or)
	}

	if bool(is_index_multi) && is_topmost_or {
		node.type = int(TokenType.IndexMulti)
	}
	return is_index_multi | int(TokenType(node.type) == .String)
}
simplify_pattern_second_pass :: proc(node: ^lib.ASTNode, options: ^QGrepOptions) {
	#partial switch TokenType(node.type) {
	case .Int:
		{
			value, ok := strconv.parse_int(node.slice)
			fmt.assertf(ok, "Failed to parse int: '%v'", node.slice)
			node.type = int(TokenType.ParsedInt)
			node.int = value
		}
	case .String:
		{
			// parse strings
			sb := lib.string_builder()
			slice := node.slice[1:len(node.slice) - 1]
			for i := 0; i < len(slice); i += 1 {
				char := slice[i]
				if char == '\\' {
					i += 1
					if i < len(slice) {
						fmt.sbprint(&sb, rune(slice[i]))
					} else {
						fmt.assertf(false, "Invalid escape")
					}
				} else {
					fmt.sbprint(&sb, rune(char))
				}
			}
			parsed_string := lib.to_string(sb)
			node.type = int(TokenType.ParsedString)
			node.str = normalize_string(parsed_string, options)
		}
	case .IndexMulti:
		// merge `or`s
		fmt.assertf(false, "TODO: merge strings into an array")
	case:
		if node.left != nil {simplify_pattern_second_pass(node.left, options)}
		if node.right != nil {simplify_pattern_second_pass(node.right, options)}
	}
}
