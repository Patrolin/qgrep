# qGrep
a windows version of grep <s>in zig-0.10.0-dev.1699+8b5d5f44e</s> in Python because Zig can't tell a file from a folder

## usage
```ps
PS C:\Users\patri\Documents\qGrep> qgrep -h
usage: qgrep.py [-h] [-d] [-c] [-a] [-s]

Search files for strings.

options:
  -h, --help  show this help message and exit
  -d          print verbose information
  -c          use case sensitive comparisons ("A" != "a")
  -a          use accent sensitive comparisons ("á" != "a")
  -s          use symbol sensitive comparisons ("ﬁ" != "fi")
```

Case insensitive by default
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep
qgrep: . "ářgůmënt"
./qgrep.py: from argparse import ArgumentParser, BooleanOptionalAction
./qgrep.py: class InvalidArgument(Exception):
./qgrep.py:                 raise InvalidArgument(f"{operator} is not a valid operator")
./qgrep.py:         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py:         argument_parser.add_argument('-d', action=BooleanOptionalAction, help='print verbose information')
./qgrep.py:         argument_parser.add_argument('-c', action=BooleanOptionalAction, help='use case sensitive comparisons')
./qgrep.py:         argument_parser.add_argument('-a', action=BooleanOptionalAction, help='use accent sensitive comparisons')
./qgrep.py:         argument_parser.add_argument('-b', action=BooleanOptionalAction, help='use symbol sensitive comparisons ("ﬁ" != "fi")')
./qgrep.py:         arguments = argument_parser.parse_args(argv[1:])
./qgrep.py:         is_debug = arguments.d
./qgrep.py:         is_case_sensitive = arguments.c
./qgrep.py:         is_accent_sensitive = arguments.a
./qgrep.py:         is_symbol_sensitive = arguments.b
./qgrep.py:         argumentsString = " " + "".join(argv[1:]) if len(argv) > 1 else ""
./qgrep.py:             match = re.search(r"(\S+)\s(.*)", input(f"qgrep{argumentsString}: ").strip())
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
./qgrep.py:         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.zig:     if (argv.len < 2) return error.NotEnoughArguments;
```

Accent sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -a
qgrep -a: . "Á"
./qgrep.py:         argument_parser.add_argument('-a', action=BooleanOptionalAction, help='use accent sensitive comparisons ("á" != "a")')
```

Symbol sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -s
qgrep -s: . "ﬁ"
./qgrep.py:         argument_parser.add_argument('-s', action=BooleanOptionalAction, help='use symbol sensitive comparisons ("ﬁ" != "fi")')
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
./qgrep.py:     acc = acc if is_accent_sensitive else "".join(v for v in acc if not unicodedata.combining(v))
./qgrep.py:     for token in tokens:
./qgrep.py:         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py:         argument_parser.add_argument('-d', action=BooleanOptionalAction, help='print verbose information')
./qgrep.py:                 for file in files:
./qgrep.py:                             for line in f.readlines():
./qgrep.zig:     while (try walker.next()) |walkerEntry| {
```
