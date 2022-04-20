#!/usr/bin/env python
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
    acc = acc if is_accent_sensitive else "".join(v for v in acc if not unicodedata.combining(v))
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

    STRING = RuleNodeTypeType(4, OperatorType.LEAF)

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
        if self.nodeType == RuleNodeType.ROOT:
            return f"{repr(self.left)}"
        elif self.nodeType == RuleNodeType.NOT:
            return f"(not {repr(self.left)})"
        elif self.nodeType == RuleNodeType.AND:
            return f"({repr(self.left)} and {repr(self.right)})"
        elif self.nodeType == RuleNodeType.OR:
            return f"({repr(self.left)} or {repr(self.right)})"
        elif self.nodeType == RuleNodeType.STRING:
            return f"\"{self.value}\""
        else:
            return ""

    def matches(self, line: str, is_case_sensitive: bool, is_accent_sensitive: bool, is_symbol_sensitive: bool) -> bool:
        if self.nodeType == RuleNodeType.ROOT:
            return self.left.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False
        elif self.nodeType == RuleNodeType.NOT:
            return not (self.left.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
                        if self.left != None else False)
        elif self.nodeType == RuleNodeType.AND:
            return (self.left.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False) \
                and (self.right.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.right != None else False)
        elif self.nodeType == RuleNodeType.OR:
            return (self.left.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False) \
                or (self.right.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.right != None else False)
        elif self.nodeType == RuleNodeType.STRING:
            if type(self.value) != str: return False
            normalized_line = normalize(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
            normalized_value = normalize(cast(str, self.value), is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
            return normalized_value in normalized_line
        else:
            return False

class InvalidOperator(Exception):
    pass

def parseRules(ruleString: str, is_debug: bool) -> RuleNode:
    tokens = re.finditer(r"(?:(\()|(\))|\"((?:\\?.)*?)\"|(\S+))", ruleString)
    root = RuleNode(RuleNodeType.ROOT)
    current = root
    for token in tokens:
        (left_bracket, right_bracket, string, operator) = token.groups()
        if left_bracket != None:
            node = RuleNode(RuleNodeType.ROOT)
            current.add(node)
            current = node
        elif right_bracket != None:
            while current.nodeType != RuleNodeType.ROOT:
                current = cast(RuleNode, current.parent)
            current = current.get_first_free_parent()
        elif string != None:
            node = RuleNode(RuleNodeType.STRING)
            node.value = string
            current.add(node)
            current = node.get_first_free_parent()
        elif operator != None and operator != "":
            if operator == "not":
                node = RuleNode(RuleNodeType.NOT)
                current.add(node)
                current = node
            elif operator == "and":
                node = RuleNode(RuleNodeType.AND)
                current.insert_left(node)
                current = node
            elif operator == "or":
                node = RuleNode(RuleNodeType.OR)
                current.insert_left(node)
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
    BLUE = "\033[1;35m"
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
            for dir_path_match in glob(dir_path):
                for (root, dirs, files) in walk(dir_path_match, topdown=True):
                    for file in files:
                        path = f"{root}/{file}".replace("\\", "/")
                        try:
                            with open(path, "r", encoding="utf8") as f:
                                if re.search(r"[\n\r]", f.read(1000)) == None:
                                    continue
                                f.seek(0, 0)
                                for line in f.readlines():
                                    if len(line) < 1000 and ruleNode.matches( \
                                        line[:-1], is_case_sensitive, is_accent_sensitive, is_symbol_sensitive
                                    ):
                                        print(f"{path}: {line[:-1]}")
                        except UnicodeDecodeError:
                            pass
    except KeyboardInterrupt:
        print("^C", end="")
        exit(0)
