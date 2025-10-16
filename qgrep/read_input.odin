package main
import "core:fmt"
import "lib"

TokenType :: enum {
	None,
	// ignore
	Whitespace,
	// brackets
	LeftBracket,
	RightBracket,
	// values
	String,
	Number,
	// unary ops
	Not,
	File,
	Line,
	// binary ops
	And,
	Or,
	Then,
	// simplified ops
	IndexMulti,
}
read_and_parse_console_input :: proc() -> ^lib.ASTNode {
	for {
		// read console input
		input := lib.read_console_input("qgrep: ")
		// parse
		parse_pattern :: proc(parser: ^lib.Parser, prev_token_type: int) -> (token: lib.Token, operator_precedence: int) {
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
				/* TODO: parse strings with backslash escapes */
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
				case "and":
					token.type = int(TokenType.And)
					operator_precedence = 1
				case "or":
					token.type = int(TokenType.Or)
				case "then":
					token.type = int(TokenType.Then)
				case:
					if TokenType(prev_token_type) == .Line {
						operator_precedence = int(lib.OpType.Value)
						token.type = int(TokenType.Number)
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
		simplify_pattern_in_place(pattern)
		lib.print_ast(pattern)
		return pattern
	}
}
simplify_pattern_in_place :: proc(node: ^lib.ASTNode, is_topmost_or := true) -> (is_index_multi: int) {
	next_is_topmost_or := is_topmost_or && TokenType(node.type) != .Or
	is_index_multi = int(TokenType(node.type) == .Or)
	if node.left != nil {
		is_index_multi &= simplify_pattern_in_place(node.left, next_is_topmost_or)
	}
	if node.right != nil {
		is_index_multi &= simplify_pattern_in_place(node.right, next_is_topmost_or)
	}

	if bool(is_index_multi) && is_topmost_or {
		node.type = int(TokenType.IndexMulti)
	}
	return is_index_multi | int(TokenType(node.type) == .String)
}
