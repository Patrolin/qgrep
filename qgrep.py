#!/usr/bin/env python3
from collections import namedtuple
from sys import argv
from argparse import ArgumentParser, BooleanOptionalAction
from glob import glob
from os import walk
from enum import Enum
import re
from typing import cast
import unicodedata

def normalize(string: str, is_case_sensitive: bool, is_accent_sensitive: bool, is_symbol_sensitive: bool) -> str:
    acc = unicodedata.normalize("NFD", string) if is_symbol_sensitive else unicodedata.normalize("NFKD", string)
    acc = acc if is_accent_sensitive else "".join(v for v in acc if not unicodedata.combining(v)) \
        # should probably keep most combining characters?

    acc = acc if is_case_sensitive else acc.lower()
    return acc

class OperatorType(Enum):
    BINARY = 0
    UNARY = 1
    LEAF = 2

RuleNodeTypeType = namedtuple("RuleNodeTypeType", ["id", "operatorType"])

class NodeIsFull(Exception):
    pass

class RuleNodeType(Enum):
    AND = RuleNodeTypeType(0, OperatorType.BINARY)
    OR = RuleNodeTypeType(1, OperatorType.BINARY)

    ROOT = RuleNodeTypeType(2, OperatorType.UNARY)
    NOT = RuleNodeTypeType(3, OperatorType.UNARY)
    FILE = RuleNodeTypeType(4, OperatorType.UNARY)

    STRING = RuleNodeTypeType(5, OperatorType.LEAF)

class RuleNode:
    def __init__(self, nodeType: RuleNodeType):
        self.parent: RuleNode | None = None
        self.left: RuleNode | None = None
        self.right: RuleNode | None = None
        self.nodeType = nodeType
        self.value: str | None = None

    def add(self, new):
        new.parent = self
        if self.left == None and (self.nodeType.value.operatorType == OperatorType.BINARY
                                  or self.nodeType.value.operatorType == OperatorType.UNARY):
            self.left = new
        elif self.right == None and self.nodeType.value.operatorType == OperatorType.BINARY:
            self.right = new
        else:
            raise NodeIsFull(repr(self))

    def insert_left(self, new):
        left_child = cast(RuleNode, self.left)
        new.parent = self
        new.left = left_child
        self.left = new
        left_child.parent = new

    def get_first_free_parent(self):
        acc = cast(RuleNode, self.parent)
        while acc.parent != None:
            if acc.nodeType.value.operatorType == OperatorType.BINARY and acc.right == None \
            or acc.nodeType.value.operatorType == OperatorType.UNARY and acc.left == None \
            or acc.nodeType == RuleNodeType.ROOT:
                break
            acc = acc.parent
        return acc

    def __repr__(self) -> str:
        if self.nodeType == RuleNodeType.AND:
            return f"({repr(self.left)} and {repr(self.right)})"
        elif self.nodeType == RuleNodeType.OR:
            return f"({repr(self.left)} or {repr(self.right)})"
        elif self.nodeType == RuleNodeType.ROOT:
            return f"{repr(self.left)}"
        elif self.nodeType == RuleNodeType.NOT:
            return f"(not {repr(self.left)})"
        elif self.nodeType == RuleNodeType.FILE:
            return f"(file {repr(self.left)})"
        elif self.nodeType == RuleNodeType.STRING:
            return f"\"{self.value}\""
        else:
            return ""

    def matches(self, filePath: str, line: str, is_case_sensitive: bool, is_accent_sensitive: bool, is_symbol_sensitive: bool) -> bool:
        if self.nodeType == RuleNodeType.AND:
            return (self.left.matches(filePath, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False) \
                and (self.right.matches(filePath, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.right != None else False)
        elif self.nodeType == RuleNodeType.OR:
            return (self.left.matches(filePath, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False) \
                or (self.right.matches(filePath, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.right != None else False)
        elif self.nodeType == RuleNodeType.NOT:
            return not (self.left.matches(filePath, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
                        if self.left != None else False)
        elif self.nodeType == RuleNodeType.FILE:
            return (self.left.matches(filePath, filePath, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
                        if self.left != None else False)
        elif self.nodeType == RuleNodeType.STRING:
            if type(self.value) != str: return False
            normalized_line = normalize(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
            normalized_value = normalize(cast(str, self.value), is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
            return normalized_value in normalized_line
        elif self.nodeType == RuleNodeType.ROOT:
            return self.left.matches(filePath, line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False
        else:
            return False

class InvalidOperator(Exception):
    pass

def parseString(s: str, i: int) -> tuple[str, int]:
    acc = ""
    if s[i] != "\"":
        return acc, -i
    while True:
        i += 1
        if i >= len(s): return acc, -i
        if s[i] == "\"": return acc, i + 1
        elif s[i] == "\\":
            i += 1
            if i >= len(s): return acc, -i
            acc += s[i]
        else:
            acc += s[i]

def tokenize(ruleString: str):
    Token = tuple[str, str, str, str]
    acc: list[Token] = []
    i = 0
    while i < len(ruleString):
        if ruleString[i] == "(":
            acc.append((ruleString[i], "", "", ""))
        elif ruleString[i] == ")":
            acc.append(("", ruleString[i], "", ""))
        elif ruleString[i] == " ":
            while ruleString[i] == " " and i < len(ruleString):
                i += 1
            continue
        else:
            if i >= len(ruleString): break
            s, j = parseString(ruleString, i)
            if j > 0:
                acc.append(("", "", s, ""))
            else:
                try:
                    j = ruleString[i:].index(" ") + i
                except ValueError:
                    j = len(ruleString)
                acc.append(("", "", "", ruleString[i:j]))
            i = j
            continue
        i += 1
    return acc

def parseRules(ruleString: str, is_debug: bool) -> RuleNode:
    tokens = tokenize(ruleString)
    #print(list(re.finditer(r"(?:(\()|(\))|\"((?:\\?.)*?)\"|(\S+))", ruleString)))
    #print("tokens", tokens)
    root = RuleNode(RuleNodeType.ROOT)
    current = root
    for token in tokens:
        (left_bracket, right_bracket, string, operator) = token
        if left_bracket != "":
            node = RuleNode(RuleNodeType.ROOT)
            current.add(node)
            current = node
        elif right_bracket != "":
            while current.nodeType != RuleNodeType.ROOT:
                current = cast(RuleNode, current.parent)
            current = current.get_first_free_parent()
        elif string != "":
            node = RuleNode(RuleNodeType.STRING)
            node.value = string
            current.add(node)
            current = node.get_first_free_parent()
        else:
            if operator == "and":
                node = RuleNode(RuleNodeType.AND)
                current.insert_left(node)
                current = node
            elif operator == "or":
                node = RuleNode(RuleNodeType.OR)
                current.insert_left(node)
                current = node
            elif operator == "not":
                node = RuleNode(RuleNodeType.NOT)
                current.add(node)
                current = node
            elif operator == "file":
                node = RuleNode(RuleNodeType.FILE)
                current.add(node)
                current = node
            else:
                raise InvalidOperator(f"{operator}")
        #if is_debug: print(f"debug: {left_bracket or right_bracket or string or operator} {current}")
    if is_debug: print(f"debug: {root}")
    return root

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
            for dir_path_match in glob(dir_path):
                any_paths = True
                for (root, dirs, files) in walk(dir_path_match, topdown=True):
                    root = root.replace("\\", "/")
                    if root == "./.git" or root.startswith("./.git/"): continue
                    for file in files:
                        path = f"{root}/{file}"
                        try:
                            with open(path, "r", encoding="utf8") as f:
                                if re.search(r"[\n\r]", f.read(1000)) == None:
                                    continue
                                f.seek(0, 0)
                                for i, line in enumerate(f.readlines()):
                                    if len(line) < 1000 and ruleNode.matches( \
                                        path, line[:-1], is_case_sensitive, is_accent_sensitive, is_symbol_sensitive
                                    ):
                                        print(f"{path}:{i+1} {line[:-1]}") # Todo(Patrolin): print {full_path}\n{line}
                        except (UnicodeDecodeError, PermissionError, OSError): # wtf
                            pass
            if not any_paths:
                print("no matching paths")
    except KeyboardInterrupt:
        print("^C", end="")
        exit(0)
