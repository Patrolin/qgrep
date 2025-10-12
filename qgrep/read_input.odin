package main
import "core:fmt"
import "lib"

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
	input := lib.read_console_input("qgrep: ")
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
