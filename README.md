# qGrep
a windows version of grep <s>in zig-0.10.0-dev.1699+8b5d5f44e</s> in Python because Zig can't tell a file from a folder

## install (windows)
After installing Python 3, Add `%appdata%\..\Local\Programs\Python\Python310` to PATH and rename python.exe to python3.exe

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
>> qgrep: . "ářgůmënt"
./qgrep.py:4; from argparse import ArgumentParser, BooleanOptionalAction
./qgrep.py:161;         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py:162;         argument_parser.add_argument('-d', action=BooleanOptionalAction, help='print verbose information')
./qgrep.py:163;         argument_parser.add_argument('-c', action=BooleanOptionalAction, help='use case sensitive comparisons ("A" != "a")')
./qgrep.py:164;         argument_parser.add_argument('-a', action=BooleanOptionalAction, help='use accent sensitive comparisons ("á" != "a")')
./qgrep.py:165;         argument_parser.add_argument('-s', action=BooleanOptionalAction, help='use symbol sensitive comparisons ("ﬁ" != "fi")')
./qgrep.py:167;         arguments = argument_parser.parse_args(argv[1:])
./qgrep.py:168;         is_debug = arguments.d
./qgrep.py:169;         is_case_sensitive = arguments.c
./qgrep.py:170;         is_accent_sensitive = arguments.a
./qgrep.py:171;         is_symbol_sensitive = arguments.s
./qgrep.py:173;         argumentsString = " " + "".join(argv[1:]) if len(argv) > 1 else ""
./qgrep.py:175;             match = re.search(r"(\S+)\s(.*)", input(f">> qgrep{argumentsString}: ").strip())
./qgrep.zig:11;     // parse arguments
./qgrep.zig:12;     if (argv.len < 2) return error.NotEnoughArguments;
```

Case sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -c
>> qgrep -c: . "Argument"
./qgrep.py:4; from argparse import ArgumentParser, BooleanOptionalAction
./qgrep.py:161;         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.zig:12;     if (argv.len < 2) return error.NotEnoughArguments;
```

Accent sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -a
>> qgrep -a: . "Á"
./qgrep.py:164;         argument_parser.add_argument('-a', action=BooleanOptionalAction, help='use accent sensitive comparisons ("á" != "a")')
```

Symbol sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -s
>> qgrep -s: . "ﬁ"
./qgrep.py:165;         argument_parser.add_argument('-s', action=BooleanOptionalAction, help='use symbol sensitive comparisons ("ﬁ" != "fi")')
```


All binary operators are left-associative with no priority
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -d
>> qgrep -d: . "for" and not "walk" or "while" and "try"
debug: ((("for" and (not "walk")) or "while") and "try")
./qgrep.zig:22;     while (try walker.next()) |walkerEntry| {
```

Bracket support
```py
>> qgrep -d: . "for" and not "walk" or ("while" and "try")
debug: (("for" and (not "walk")) or ("while" and "try"))
./qgrep.py:14;     acc = acc if is_accent_sensitive else "".join(v for v in acc if not unicodedata.combining(v))
./qgrep.py:113;     for token in tokens:
./qgrep.py:161;         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py:162;         argument_parser.add_argument('-d', action=BooleanOptionalAction, help='print verbose information')
./qgrep.py:185;             for dir_path_match in glob(dir_path):
./qgrep.py:187;                     for file in files:
./qgrep.py:194;                                 for i, line in enumerate(f.readlines()):
./qgrep.zig:22;     while (try walker.next()) |walkerEntry| {
```
