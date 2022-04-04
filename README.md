# qGrep
a windows version of grep <s>in zig-0.10.0-dev.1699+8b5d5f44e</s> in Python because Zig can't tell a file from a folder

## usage
```ps
PS C:\Users\Patrolin\Documents\qGrep> qgrep -h
usage: qgrep.py [-h] [-debug] [-c]

Search files for strings.

options:
  -h, --help  show this help message and exit
  -d          print verbose information
  -c          use case sensitive comparisons
```

Case insensitive by default
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep
qgrep: . "argument"
./qgrep.py: from argparse import ArgumentParser, BooleanOptionalAction
./qgrep.py: class InvalidArgument(Exception):
./qgrep.py:                 raise InvalidArgument(f"{operator} is not a valid operator")
./qgrep.py: argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py: argument_parser.add_argument('-d', action=BooleanOptionalAction, help='print verbose information')
./qgrep.py: argument_parser.add_argument('-c', action=BooleanOptionalAction, help='use case sensitive comparisons')
./qgrep.py:     arguments = argument_parser.parse_args(argv[1:])
./qgrep.py:     is_debug = arguments.d
./qgrep.py:     is_case_sensitive = arguments.c
./qgrep.py:     argumentsString = " " + "".join(argv[1:]) if len(argv) > 1 else ""
./qgrep.py:         match = re.search(r"(\S+)\s(.*)", input(f"qgrep{argumentsString}: ").strip())
./qgrep.zig:     // parse arguments
./qgrep.zig:     if (argv.len < 2) return error.NotEnoughArguments;
```

Case sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -c
qgrep -c: . "Argument"
./qgrep.py: from argparse import ArgumentParser, BooleanOptionalAction
./qgrep.py: class InvalidArgument(Exception):
./qgrep.py:                 raise InvalidArgument(f"{operator} is not a valid operator")
./qgrep.py: argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.zig:     if (argv.len < 2) return error.NotEnoughArguments;
```

All binary operators are left-associative with no priority
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -d
qgrep -d: . "for" and not "walk" or "while" and "try"
debug: ((("for" and (not "walk")) or "while") and "try")
./qgrep.zig:     while (try walker.next()) |walkerEntry| {
```

Bracket support
```py
qgrep -d: . "for" and not "walk" or ("while" and "try")
debug: (("for" and (not "walk")) or ("while" and "try"))
./qgrep.py:     for token in tokens:
./qgrep.py: argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py: argument_parser.add_argument('-d', action=BooleanOptionalAction, help='print verbose information')
./qgrep.py:             for file in files:
./qgrep.py:                         for line in f.readlines():
./qgrep.zig:     while (try walker.next()) |walkerEntry| {
```
