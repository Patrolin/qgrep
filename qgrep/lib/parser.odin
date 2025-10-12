package lib
import "core:fmt"

TokenType :: int
OpType :: enum {
	Ignore = -1,
	Value  = -2,
	Unary  = -3,
}

Parser :: struct {
	str:         string,
	start:       int,
	parser_proc: ParserProc `fmt:"-"`,
	keep_going:  bool,
	error:       string,
}
ASTNode :: struct {
	using token: Token,
	left, right: ^ASTNode,
}
Token :: struct {
	slice: string,
	type:  TokenType,
}
ParserProc :: proc(parser: ^Parser, prev_token_type: TokenType) -> (token: Token, operator_precedence: int)

@(private = "file")
_parser_eat_token :: #force_inline proc(parser: ^Parser, token: Token) {
	parser.start += len(token.slice)
	parser.keep_going = parser.start < len(parser.str)
}
report_parser_error :: proc(parser: ^Parser, error: string) {
	parser.keep_going = false
	parser.error = error
}

@(private = "file")
_parse_downwards :: proc(parser: ^Parser, prev_node: ^ASTNode, min_precedence: int, allocator := context.temp_allocator) -> (node: ^ASTNode) {
	prev_node_unary_tail := prev_node
	prev_node_unary_tail_is_value := false
	prev_node := prev_node
	token: Token
	node = new(ASTNode, allocator = allocator)
	for parser.keep_going {
		token, operator_precedence := parser.parser_proc(parser, token.type)
		fmt.printfln("token: %v, %v", token, operator_precedence)
		if len(token.slice) == 0 {
			report_parser_error(parser, "Cannot have token of length 0")
		}
		switch OpType(operator_precedence) {
		case OpType.Ignore:
			_parser_eat_token(parser, token)
		case OpType.Value, OpType.Unary:
			node.token = token
			if prev_node_unary_tail_is_value {
				report_parser_error(parser, "Cannot have two values in a row")
				return
			}
			_parser_eat_token(parser, token)
			if prev_node == nil {
				prev_node = node
			} else {
				prev_node_unary_tail.left = node
			}
			prev_node_unary_tail = node
			prev_node_unary_tail_is_value = OpType(operator_precedence) == .Value
			node = new(ASTNode, allocator = allocator)
		case:
			// binary
			if prev_node == nil {
				report_parser_error(parser, "Cannot have binary op without a value")
				return
			}
			if operator_precedence < min_precedence {break}
			_parser_eat_token(parser, token)
			node.token = token
			node.left = prev_node
			node.right = _parse_upwards(parser, operator_precedence, allocator = allocator)
			prev_node = node
			node = new(ASTNode, allocator = allocator)
		}
	}
	return prev_node
}
@(private = "file")
_parse_upwards :: proc(parser: ^Parser, min_precedence: int, allocator := context.temp_allocator) -> (prev_node: ^ASTNode) {
	for parser.keep_going {
		prev_node = _parse_downwards(parser, prev_node, min_precedence, allocator = allocator)
	}
	return prev_node
}
parse :: proc(str: string, parser_proc: ParserProc, allocator := context.temp_allocator) -> ^ASTNode {
	parser := Parser {
		str         = str,
		start       = 0,
		parser_proc = parser_proc,
		keep_going  = true,
		error       = "",
	}
	result := _parse_upwards(&parser, 0, allocator = allocator)
	if len(parser.error) > 0 {fmt.printfln("Error: %v", parser.error)}
	return result
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
