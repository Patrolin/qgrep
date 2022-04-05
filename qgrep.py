#!/usr/bin/env python
from ast import arg
from sys import argv
from argparse import ArgumentParser, BooleanOptionalAction
from decimal import InvalidOperation
from os import walk
import re
from typing import cast
import unicodedata

def normalize(string: str, is_case_sensitive: bool, is_accent_sensitive: bool, is_symbol_sensitive: bool) -> str:
    acc = unicodedata.normalize("NFD", string) if is_symbol_sensitive else unicodedata.normalize("NFKD", string)
    acc = acc if is_accent_sensitive else "".join(v for v in acc if not unicodedata.combining(v))
    acc = acc if is_case_sensitive else acc.lower()
    return acc

class RuleNode:
    def __init__(self, nodeType: int):
        self.parent: RuleNode | None = None
        self.left: RuleNode | None = None
        self.right: RuleNode | None = None
        self.nodeType = nodeType
        self.value: str | None = None

    def add(self, new):
        ignoreLeft = self.nodeType == 2
        new.parent = self
        if self.left == None and not ignoreLeft:
            self.left = new
        elif self.right == None:
            self.right = new
        else:
            raise InvalidOperation()
        return new

    def insert_above(self, new):
        parent = cast(RuleNode, self.parent)
        new.parent = parent
        new.left = self
        parent.left = new
        self.parent = new

    def insert_below(self, new):
        left_child = cast(RuleNode, self.left)
        new.parent = self
        new.left = left_child
        self.left = new
        left_child.parent = new

    def get_first_empty_parent(self):
        acc = cast(RuleNode, self.parent)
        while acc.parent != None and acc.right != None:
            acc = acc.parent
        return acc

    def __repr__(self) -> str:
        if self.nodeType == 0:
            return f"{repr(self.left)}"
        elif self.nodeType == 1:
            return f"\"{self.value}\""
        elif self.nodeType == 2:
            return f"(not {repr(self.right)})"
        elif self.nodeType == 3:
            return f"({repr(self.left)} and {repr(self.right)})"
        elif self.nodeType == 4:
            return f"({repr(self.left)} or {repr(self.right)})"
        else:
            return ""

    def matches(self, line: str, is_case_sensitive: bool, is_accent_sensitive: bool, is_symbol_sensitive: bool) -> bool:
        if self.nodeType == 0: # passthrough
            return self.left.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False
        elif self.nodeType == 1: # string
            if type(self.value) != str: return False
            line2 = normalize(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
            value2 = normalize(cast(str, self.value), is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
            return value2 in line2
        elif self.nodeType == 2: # not
            return not (self.right.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive)
                        if self.right != None else False)
        elif self.nodeType == 3: # and
            return (self.left.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False) \
                and (self.right.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.right != None else False)
        elif self.nodeType == 4: # or
            return (self.left.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.left != None else False) \
                or (self.right.matches(line, is_case_sensitive, is_accent_sensitive, is_symbol_sensitive) if self.right != None else False)
        else:
            return False

class InvalidArgument(Exception):
    pass

def parseRules(ruleString: str, is_debug: bool) -> RuleNode:
    tokens = re.finditer(r"(?:(\()|(\))|\"((?:\\?.)*?)\"|(\S+))", ruleString)
    root = RuleNode(0)
    current = root
    for token in tokens:
        (left_bracket, right_bracket, string, operator) = token.groups()
        if left_bracket != None:
            node = RuleNode(0)
            current.add(node)
            current = node
        elif right_bracket != None:
            while current.nodeType != 0:
                current = cast(RuleNode, current.parent)
            current = current.get_first_empty_parent()
        elif string != None:
            node = RuleNode(1)
            node.value = string
            current.add(node)
            current = node.get_first_empty_parent()
        elif operator != None and operator != "":
            if operator == "not":
                node = RuleNode(2)
                current.add(node)
                current = node
            elif operator == "and":
                node = RuleNode(3)
                current.insert_below(node)
                current = node
            elif operator == "or":
                node = RuleNode(4)
                current.insert_below(node)
                current = node
            else:
                raise InvalidArgument(f"{operator} is not a valid operator")
        #if is_debug: print(f"debug: {left_bracket or right_bracket or string or operator} {current}")
    if is_debug: print(f"debug: {root}")
    return root

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
            match = re.search(r"(\S+)\s(.*)", input(f"qgrep{argumentsString}: ").strip())
            if match == None:
                print("usage: <relative path> <rules>")
                continue
            dir_path, ruleString = match.groups()
            try:
                ruleNode = parseRules(ruleString, is_debug)
            except Exception as error:
                print(error)
                continue
            for (root, dirs, files) in walk(dir_path, topdown=True):
                for file in files:
                    path = f"{root}/{file}".replace("\\", "/")
                    try:
                        with open(path, "r", encoding="utf8") as f:
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
