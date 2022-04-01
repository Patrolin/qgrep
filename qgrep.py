
from sys import argv
from os import walk
from os.path import abspath
import re

class NotEnoughArguments(Exception):
    pass

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
    if len(argv) < 2:
        raise NotEnoughArguments();
    rules = [Rule(v) for v in argv[1:]]
    for (root, dirs, files) in walk(".", topdown = True):
        for file in files:
            path = f"{root}/{file}".replace("\\", "/")
            try:
                with open(path, "r", encoding="utf8") as f:
                    for line in f.readlines():
                        if all(rule.matches(line[:-1]) for rule in rules):
                            print(f"{path}: {line[:-1]}")
            except UnicodeDecodeError:
                pass
