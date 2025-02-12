#!/usr/bin/env python3
from sys import argv
from argparse import ArgumentParser, BooleanOptionalAction
from glob import glob
from os import walk
from os.path import getsize as getFileSize
from enum import Enum
import re
from typing import Any, cast
import unicodedata

# lexer
class TokenType(Enum):
    # leaf
    STRING = 0
    # unary
    NOT = 1
    FILE = 2
    LINE = 3,
    LBRACKET = 4,
    RBRACKET = 5,
    # binary
    AND = 6
    OR = 7
class Slice:
    def __init__(self, ptr: str, start: int|None = None, end: int|None = None):
        self.ptr = ptr
        self.start = start or 0
        self.end = end or len(self.ptr)
    def __repr__(self):
        return self.ptr[self.start:self.end]
    def __getitem__(self, key: int|slice) -> str:
        if isinstance(key, slice):
            start = self.start + key.start
            end = self.start + (key.stop or 0)
            return self.ptr[slice(start, end, key.step)]
        else:
            if key >= self.end: raise IndexError("slice index out of range")
            return self.ptr[self.start + key]
    def __len__(self):
        return self.end - self.start
class Token:
    def __init__(self, slice_: Slice, type_: TokenType):
        self.slice = slice_
        self.type = type_
    def __repr__(self):
        return f"Token({self.type}, {self.slice.start}, {self.slice.end})"
def lexStringToken(slice_: Slice):
    i = 0
    while i < len(slice_) and slice_[i] != '"':
        i += 1 + (slice_[i] == '\\')
    i = min(i, len(slice_)-1)
    if slice_[i] != '"': raise ValueError(f"Unclosed string: {slice_}")
    return Token(Slice(slice_.ptr, slice_.start, slice_.start+i), TokenType.STRING)
def lexTokens(slice_: Slice) -> list[Token]:
    acc_tokens: list[Token] = []
    i = 0
    while True:
        while i < len(slice_) and slice_[i] == " ":
            i += 1
        if i >= len(slice_): break
        if slice_[i] == '"':
            string_token = lexStringToken(Slice(slice_.ptr, i+1, len(slice_.ptr)))
            acc_tokens.append(string_token)
            i = string_token.slice.end + 1
        elif slice_[i:i+3] == "and":
            acc_tokens.append(Token(Slice(slice_.ptr, slice_.start+i, slice_.start+i+3), TokenType.AND))
            i += 3
        elif slice_[i:i+2] == "or":
            acc_tokens.append(Token(Slice(slice_.ptr, slice_.start+i, slice_.start+i+2), TokenType.OR))
            i += 2
        elif slice_[i:i+3] == "not":
            acc_tokens.append(Token(Slice(slice_.ptr, slice_.start+i, slice_.start+i+3), TokenType.NOT))
            i += 3
        elif slice_[i:i+4] == "file":
            acc_tokens.append(Token(Slice(slice_.ptr, slice_.start+i, slice_.start+i+4), TokenType.FILE))
            i += 4
        elif slice_[i:i+4] == "line":
            acc_tokens.append(Token(Slice(slice_.ptr, slice_.start+i, slice_.start+i+4), TokenType.LINE))
            i += 4
        elif slice_[i] == "(":
            acc_tokens.append(Token(Slice(slice_.ptr, slice_.start+i, slice_.start+i+1), TokenType.LBRACKET))
            i += 1
        elif slice_[i] == ")":
            acc_tokens.append(Token(Slice(slice_.ptr, slice_.start+i, slice_.start+i+1), TokenType.RBRACKET))
            i += 1
        else:
            raise ValueError(f"invalid token: {slice_.ptr[i:]}")
    return acc_tokens

# parser
class TokenView:
    def __init__(self, tokens: list[Token]):
        self.tokens = tokens
        self.i = 0
    def __repr__(self):
        return f"TokenView({self.i}, {self.tokens})"
    def nextToken(self) -> Token|None:
        return self.tokens[self.i] if self.i < len(self.tokens) else None
    def lookAhead(self, j: int) -> Token|None:
        return self.tokens[self.i + j] if self.i + j < len(self.tokens) else None
    def eatToken(self):
        self.i += 1
class RuleNode:
    def __init__(self, type_: TokenType, value: Any = None):
        self.type = type_
        self.value = value
        self.left: RuleNode | None = None
        self.right: RuleNode | None = None

    def __repr__(self) -> str:
        if self.type == TokenType.AND:
            return f"({repr(self.left)} and {repr(self.right)})"
        elif self.type == TokenType.OR:
            return f"({repr(self.left)} or {repr(self.right)})"
        elif self.type == TokenType.NOT:
            return f"(not {repr(self.left)})"
        elif self.type == TokenType.FILE:
            return f"(file {repr(self.left)})"
        elif self.type == TokenType.LINE:
            return f"(line {repr(self.left)})"
        elif self.type == TokenType.STRING:
            return f"\"{self.value}\""
        else:
            return ""

    def matches(self, filePath: str, lineNumber: int, line: str, is_case_sensitive: bool, is_accent_sensitive: bool, is_symbol_sensitive: bool) -> bool:
        if self.type == TokenType.AND:
            return (self.left.matches(filePath, lineNumber, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False) \
                and (self.right.matches(filePath, lineNumber, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.right != None else False)
        elif self.type == TokenType.OR:
            return (self.left.matches(filePath, lineNumber, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False) \
                or (self.right.matches(filePath, lineNumber, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.right != None else False)
        elif self.type == TokenType.NOT:
            return not (self.left.matches(filePath, lineNumber, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
                        if self.left != None else False)
        elif self.type == TokenType.FILE:
            return (self.left.matches(filePath, lineNumber, filePath, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
                        if self.left != None else False)
        elif self.type == TokenType.LINE:
            wanted_line_string = str(self.left).removeprefix('"').removesuffix('"')
            try:
                wanted_line = int(wanted_line_string)
                return lineNumber == wanted_line
            except ValueError:
                pass
            raise ValueError(f"invalid line number: {self.left}")
        elif self.type == TokenType.STRING:
            if type(self.value) != str: return False
            normalized_line = normalize(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
            normalized_value = normalize(cast(str, self.value), is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
            return normalized_value in normalized_line
        else:
            return False
def isUnary(token: Token) -> bool:
    return (token.type == TokenType.NOT) or (token.type == TokenType.FILE) or (token.type == TokenType.LINE)
def parseLeafOrUnary(tokens: TokenView) -> RuleNode|None:
    next_token = tokens.nextToken()
    if next_token == None:
        return None
    elif next_token.type == TokenType.STRING:
        acc_string_value = ""
        i = 0
        while i < len(next_token.slice):
            if next_token.slice[i] == "\\":
                acc_string_value += next_token.slice[i+1]
                i += 2
            else:
                acc_string_value += next_token.slice[i]
                i += 1
        tokens.eatToken()
        return RuleNode(TokenType.STRING, acc_string_value)
    elif isUnary(next_token):
        node = RuleNode(next_token.type)
        tokens.eatToken()
        node.left = parseLeafOrUnary(tokens)
        return node
    elif next_token.type == TokenType.LBRACKET:
        left_bracket_i = tokens.i
        tokens.eatToken()
        node = parseBinary(tokens, -1)
        next_token = tokens.nextToken()
        if (next_token == None) or (next_token.type != TokenType.RBRACKET): raise ValueError(f"mismatched brackets: {tokens.tokens[left_bracket_i:]}")
        tokens.eatToken()
        return node
    raise ValueError(f"invalid leaf token: {next_token}")
def isBinary(token: Token) -> bool:
    return (token.type == TokenType.AND) or (token.type == TokenType.OR)
def getPrecedence(token: Token) -> int:
    return 0 # make everything be parsed left-to-right
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

def normalize(string: str, is_case_sensitive: bool, is_accent_sensitive: bool, is_symbol_sensitive: bool) -> str:
    acc = unicodedata.normalize("NFD", string) if is_symbol_sensitive else unicodedata.normalize("NFKD", string)
    acc = acc if is_accent_sensitive else "".join(v for v in acc if not unicodedata.combining(v))
    acc = acc if is_case_sensitive else acc.lower()
    return acc

def parseRules(ruleString: str, is_debug: bool) -> RuleNode:
    tokens = lexTokens(Slice(ruleString, 0, len(ruleString)))
    token_view = TokenView(tokens)
    rules = parseBinary(token_view, -1)
    if token_view.i < len(tokens): raise ValueError(f"mismatched token {tokens[token_view.i:]}")
    if rules == None: raise ValueError("empty ruleString")
    if is_debug: print(f"debug: {rules}")
    return rules

class TextColor:
    # Powershell has VT codes disabled by default...
    BLACK = "\033[1;31m"
    RED = "\033[1;32m"
    GREEN = "\033[1;33m"
    YELLOW = "\033[1;34m"
    BLUE = "\033[1;35m" # Todo(Patrolin): this is not blue
    MAGENTA = "\033[1;36m"
    CYAN = "\033[1;37m"
    WHITE = "\033[1;38m"
    RESET = "\033[1;39m"

KIBI_BYTE = 1024
MEBI_BYTE = 1024 * KIBI_BYTE
MAX_FILE_SIZE = 20 * MEBI_BYTE
if __name__ == "__main__":
    try:
        argument_parser = ArgumentParser(description='Search files for strings.')
        argument_parser.add_argument('-d', action=BooleanOptionalAction, help='print verbose information')
        argument_parser.add_argument('-c', action=BooleanOptionalAction, help='use case sensitive comparisons ("A" != "a")')
        argument_parser.add_argument('-a', action=BooleanOptionalAction, help='use accent sensitive comparisons ("á" != "a")')
        argument_parser.add_argument('-s', action=BooleanOptionalAction, help='use symbol sensitive comparisons ("ﬁ" != "fi")')

        arguments = argument_parser.parse_args(argv[1:])
        is_debug = arguments.d
        is_case_sensitive = arguments.c
        is_accent_sensitive = arguments.a
        is_symbol_sensitive = arguments.s

        argumentsString = " " + "".join(argv[1:]) if len(argv) > 1 else ""
        while True:
            match = re.search(r"(\S+)\s(.*)", input(f">> qgrep{argumentsString}: ").strip())
            if match == None:
                print("usage: <relative path> <rules>")
                continue
            dir_path, ruleString = match.groups()
            try:
                ruleNode = parseRules(ruleString, is_debug)
            except Exception as error:
                print(repr(error))
                continue
            any_paths = False
            did_skip_node_modules = False
            for dir_path_match in glob(dir_path):
                any_paths = True
                for (root, dirs, files) in walk(dir_path_match, topdown=True):
                    root = root.replace("\\", "/")
                    if root == "./.git" or root.startswith("./.git/"): continue
                    for dir_ in dirs:
                        if dir_ == "node_modules":
                            if not did_skip_node_modules:
                                print(f"skipping all */node_modules/*")
                                did_skip_node_modules = True
                            dirs.remove(dir_)
                            continue
                    for file in files:
                        path = f"{root}/{file}"
                        try:
                            file_size = getFileSize(path) # NOTE: this can also throw OsError
                            if file_size > MAX_FILE_SIZE:
                                print(f"skipping {path}")
                                continue
                            with open(path, "r", encoding="utf8") as f:
                                if re.search(r"[\n\r]", f.read(1000)) == None:
                                    continue
                                f.seek(0, 0)
                                for i, line in enumerate(f.readlines()):
                                    if len(line) < 1000 and ruleNode.matches( \
                                        path, i+1, line[:-1], is_case_sensitive, is_accent_sensitive, is_symbol_sensitive
                                    ):
                                        print(f"{path}:{i+1} {line[:-1]}") # Todo(Patrolin): print {full_path}\n{line}
                        except (UnicodeDecodeError, PermissionError, OSError): # wtf
                            pass
            if not any_paths:
                print("no matching paths")
    except KeyboardInterrupt:
        print("^C", end="")
        exit(0)

# TODO: add line operator
