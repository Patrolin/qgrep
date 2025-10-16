package lib
import "base:intrinsics"
import "core:fmt"

TokenType :: int
OpType :: enum {
	Ignore       = -1,
	Value        = -2,
	Unary        = -3,
	LeftBracket  = -4,
	RightBracket = -5,
}

Parser :: struct {
	str:           string,
	start:         int,
	parser_proc:   ParserProc `fmt:"-"`,
	keep_going:    bool,
	error:         string,
	bracket_count: int,
}
ASTNode :: struct {
	using token: Token,
	left, right: ^ASTNode,
}
#assert(size_of(ASTNode) == 40)

Token :: struct {
	slice: string,
	type:  TokenType,
}
#assert(size_of(Token) == 24)

ParserProc :: proc(parser: ^Parser, prev_token_type: TokenType) -> (token: Token, operator_precedence: int)

@(private = "file")
_parser_eat_token :: #force_inline proc(parser: ^Parser, token: Token) {
	parser.start += len(token.slice)
	parser.keep_going = parser.start < len(parser.str)
}
report_parser_error :: proc(parser: ^Parser, error: string) {
	parser.keep_going = false
	if parser.error == "" {parser.error = error}
}

/* parse `A + B * ...` as `A + [B * ...]` */
@(private = "file")
_parse_downwards :: proc(parser: ^Parser, prev_node: ^ASTNode, min_precedence: int, allocator := context.temp_allocator) -> (node: ^ASTNode) {
	/* NOTE: unary head or binary op */
	prev_node := prev_node
	prev_node_unary_tail := prev_node
	prev_node_unary_tail_is_value := false
	token: Token
	for parser.keep_going {
		token, operator_precedence := parser.parser_proc(parser, token.type)
		//fmt.printfln("_parse_downwards: %v, %v", token, operator_precedence)
		if intrinsics.expect(len(token.slice) == 0 || !parser.keep_going, false) {
			report_parser_error(parser, "Cannot have token of length 0")
			break
		}
		switch OpType(operator_precedence) {
		case OpType.Ignore:
			_parser_eat_token(parser, token)
		case OpType.Value, OpType.Unary:
			node = new(ASTNode, allocator = allocator)
			node.token = token
			if prev_node_unary_tail_is_value {
				report_parser_error(parser, "Cannot have two values in a row")
				break
			}
			_parser_eat_token(parser, token)
			if prev_node == nil {
				prev_node = node
			} else {
				prev_node_unary_tail.left = node
			}
			prev_node_unary_tail = node
			prev_node_unary_tail_is_value = OpType(operator_precedence) == .Value
		case OpType.LeftBracket:
			// left bracket
			_parser_eat_token(parser, token)
			parser.bracket_count += 1
			prev_node = _parse_upwards(parser, -1, allocator = allocator)
			// right bracket
			token, operator_precedence := parser.parser_proc(parser, token.type)
			if OpType(operator_precedence) != .RightBracket {
				report_parser_error(parser, "Unclosed left bracket")
				break
			}
			parser.bracket_count -= 1
			_parser_eat_token(parser, token)
		case OpType.RightBracket:
			if parser.bracket_count == 0 {
				report_parser_error(parser, "Unclosed right bracket")
			}
			// close until we find the matching left bracket
			parser.keep_going = false
			break
		case:
			// binary
			if prev_node == nil {
				report_parser_error(parser, "Cannot have binary op without a value")
				break
			}
			if operator_precedence <= min_precedence {
				/* NOTE: if `operator_precedence == min_precedence` we always do left-to-right associative to reduce confusion */
				parser.keep_going = false
				break
			}
			_parser_eat_token(parser, token)
			node = new(ASTNode, allocator = allocator)
			node.token = token
			node.left = prev_node
			node.right = _parse_upwards(parser, operator_precedence, allocator = allocator)
			prev_node = node
		}
	}
	return prev_node
}
/* parse `A * B + ...` as `[A * B] + ...` */
@(private = "file")
_parse_upwards :: proc(parser: ^Parser, min_precedence: int, allocator := context.temp_allocator) -> (prev_node: ^ASTNode) {
	for parser.keep_going {
		//fmt.printfln("_parse_upwards: '%v', %v, %v", parser.str[parser.start:], min_precedence, prev_node)
		prev_node = _parse_downwards(parser, prev_node, min_precedence, allocator = allocator)
	}
	/* NOTE: reset after right bracket, or binary op with lower precedence */
	parser.keep_going = parser.error == "" && parser.start < len(parser.str)
	return prev_node
}
parse :: proc(str: string, parser_proc: ParserProc, allocator := context.temp_allocator) -> (node: ^ASTNode, error: string) {
	parser := Parser {
		str         = str,
		start       = 0,
		parser_proc = parser_proc,
		keep_going  = true,
		error       = "",
	}
	result := _parse_upwards(&parser, -1, allocator = allocator)
	return result, parser.error
}
print_ast :: proc(node: ^ASTNode, indent: int = 0) {
	indent_str := repeat(" ", indent)
	fmt.printfln("%v", node.token)
	if node.left != nil {
		fmt.printf("%v- ", indent_str)
		print_ast(node.left, indent + 1)
	}
	if node.right != nil {
		fmt.printf("%v- ", indent_str)
		print_ast(node.right, indent + 1)
	}
}
