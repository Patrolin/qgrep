package main

// `"foo" and "bar" or "zee"`
// `"foo" then "bar"`
//
//  V
//
//    And
//  /    \
// "on"  "off"

TokenType :: enum {
	None,
	// values
	Number,
	String,
	// binary ops
	And,
	Or,
	Then,
}
Token :: struct {
	type:        TokenType,
	left, right: ^Token,
}
Parser :: struct {
	str:   string,
	start: int,
}
ParsedToken :: struct {}
ParserProc :: proc(parser: ^Parser, prev_token_type: TokenType) -> ParsedToken
parse_binary :: proc(parser: ^Parser, min_precedence: int) {
	//next_token := ?
	if next_token == nil || !is_binary_operator(next_token) {return left}
	next_precedence = get_operator_precedence(next_token)
	if next_precedence <= min_precedence {return left}
	// parse left
	tokens.eatToken()
	right = parse_binary(tokens, next_precedence)
	node = RuleNode(next_token.type)
	node.left = left
	node.right = right
	return node
}
parse :: proc(parser: ^Parser, parse_next: ParserProc) {
	token := parse_next(parser, TokenType.None)
	for {

	}
}
/*

def parseBinaryRightwards(tokens: TokenView, left: RuleNode, minPrecedence: int) -> RuleNode:
  next_token = tokens.nextToken()
  if next_token == None or not isBinary(next_token): return left
  next_precedence = getPrecedence(next_token)
  if next_precedence <= minPrecedence: return left
  tokens.eatToken()
  right = parseBinary(tokens, next_precedence)
  node = RuleNode(next_token.type)
  node.left = left
  node.right = right
  return node
def parseBinary(tokens: TokenView, minPrecedence: int) -> RuleNode|None:
  left = parseLeafOrUnary(tokens)
  while True:
    node = parseBinaryRightwards(tokens, left, minPrecedence)
    if node == left: break
    left = node
  return left
/*
