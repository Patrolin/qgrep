from decimal import InvalidOperation
from os import walk
import re
from typing import cast

class RuleNode:
    def __init__(self, nodeType: int):
        self.parent: RuleNode | None = None
        self.left: RuleNode | None = None
        self.right: RuleNode | None = None
        self.nodeType = nodeType
        self.value: str | None = None

    def add(self, new):
        new.parent = self
        if self.left == None:
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

    def __repr__(self) -> str:
        if self.nodeType == 0:
            return f"{repr(self.left)}"
        elif self.nodeType == 1:
            return f"\"{self.value}\""
        elif self.nodeType == 2:
            return f"(not {repr(self.left)})"
        elif self.nodeType == 3:
            return f"({repr(self.left)} and {repr(self.right)})"
        elif self.nodeType == 4:
            return f"({repr(self.left)} or {repr(self.right)})"
        else:
            return ""

    def matches(self, line: str) -> bool:
        if self.nodeType == 0: # passthrough
            return self.left.matches(line) if self.left != None else False
        elif self.nodeType == 1: # string
            return cast(str, self.value) in line if type(self.value) == str else False
        elif self.nodeType == 2: # not
            return not (self.left.matches(line) if self.left != None else False)
        elif self.nodeType == 3: # and
            return (self.left.matches(line) if self.left != None else False) \
                and (self.right.matches(line) if self.right != None else False)
        elif self.nodeType == 4: # or
            return (self.left.matches(line) if self.left != None else False) \
                or (self.right.matches(line) if self.right != None else False)
        else:
            return False

class InvalidArgument(Exception):
    pass

def parseRules(ruleString: str) -> RuleNode:
    tokens = re.finditer(r"(?:(\()|(\))|\"((?:\\?.)*?)\"|(\S*))", ruleString)
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
            current = cast(RuleNode, current.parent)
        elif string != None:
            node = RuleNode(1)
            node.value = string
            current.add(node)
            current = node
            while cast(RuleNode, current.parent).nodeType == 2:
                current = cast(RuleNode, current.parent)
        elif operator != None and operator != "":
            if operator == "not":
                node = RuleNode(2)
                current.add(node)
                current = node
            elif operator == "and":
                node = RuleNode(3)
                current.insert_above(node)
                current = node
            elif operator == "or":
                node = RuleNode(4)
                current.insert_above(node)
                current = node
            else:
                raise InvalidArgument(f"{operator} is not a valid operator")
    print(root)
    return root

class Rule:
    def __init__(self, ruleString: str):
        if ruleString.startswith("-"):
            self.ruleType = 0
            self.pattern = ruleString[1:]
        else:
            self.ruleType = 1
            self.pattern = ruleString

    def matches(self, string: str) -> bool:
        return (re.search(self.pattern, string) != None) ^ (self.ruleType == 0)

    def __repr__(self) -> str:
        return f"Rule({self.ruleType}, {self.pattern})"

if __name__ == "__main__":
    while True:
        match = re.search(r"(\S+)\s(.*)", input("qgrep: ").strip())
        if match == None: continue
        dir_path, ruleString = match.groups()
        ruleNode = parseRules(ruleString)
        for (root, dirs, files) in walk(dir_path, topdown=True):
            for file in files:
                path = f"{root}/{file}".replace("\\", "/")
                try:
                    with open(path, "r", encoding="utf8") as f:
                        for line in f.readlines():
                            if len(line) < 1000 and ruleNode.matches(line[:-1]):
                                print(f"{path}: {line[:-1]}")
                except UnicodeDecodeError:
                    pass
