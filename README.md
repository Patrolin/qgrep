# qGrep
a windows version of grep in Python

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
  -u          only print unique lines
```

Case insensitive by default
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep
>> qgrep: . "ářgůmënt" and file "qgrep.py"
./qgrep.py:3 from argparse import ArgumentParser, BooleanOptionalAction
./qgrep.py:242         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py:243         argument_parser.add_argument('-d', action=BooleanOptionalAction, help='print verbose information')
./qgrep.py:244         argument_parser.add_argument('-c', action=BooleanOptionalAction, help='use case sensitive comparisons ("A" != "a")')
./qgrep.py:245         argument_parser.add_argument('-a', action=BooleanOptionalAction, help='use accent sensitive comparisons ("á" != "a")')
./qgrep.py:246         argument_parser.add_argument('-s', action=BooleanOptionalAction, help='use symbol sensitive comparisons ("ﬁ" != "fi")')
./qgrep.py:248         arguments = argument_parser.parse_args(argv[1:])
./qgrep.py:249         is_debug = arguments.d
./qgrep.py:250         is_case_sensitive = arguments.c
./qgrep.py:251         is_accent_sensitive = arguments.a
./qgrep.py:252         is_symbol_sensitive = arguments.s
./qgrep.py:254         argumentsString = " " + "".join(argv[1:]) if len(argv) > 1 else ""
./qgrep.py:256             match = re.search(r"(\S+)\s(.*)", input(f">> qgrep{argumentsString}: ").strip())
```

Case sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -c
>> qgrep -c: . "Argument" and file "grep.py"
./qgrep.py:3 from argparse import ArgumentParser, BooleanOptionalAction
./qgrep.py:242         argument_parser = ArgumentParser(description='Search files for strings.')
```

Accent sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -a
>> qgrep -a: . "Á" and file "grep.py"
./qgrep.py:245         argument_parser.add_argument('-a', action=BooleanOptionalAction, help='use accent sensitive comparisons ("á" != "a")')
```

Symbol sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -s
>> qgrep -s: . "ﬁ" and file "grep.py"
./qgrep.py:246         argument_parser.add_argument('-s', action=BooleanOptionalAction, help='use symbol sensitive comparisons ("ﬁ" != "fi")')
```

All binary operators are left-associative with no priority
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -d
>> qgrep -d: . "with " or "for " and file "qgrep.py"
debug: (("with " or "for ") and (file "qgrep.py"))
./qgrep.py:212     acc = acc if is_accent_sensitive else "".join(v for v in acc if not unicodedata.combining(v))
./qgrep.py:242         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py:267             for dir_path_match in glob(dir_path):
./qgrep.py:269                 for (root, dirs, files) in walk(dir_path_match, topdown=True):
./qgrep.py:272                     for file in files:
./qgrep.py:279                             with open(path, "r", encoding="utf8") as f:
./qgrep.py:283                                 for i, line in enumerate(f.readlines()):
```

Bracket support
```py
>> qgrep -d: . "for" and not "walk" or ("while" and "try") and file "www"
debug: ((("for" and (not "walk")) or ("while" and "try")) and (file "www"))
```

File search
```py
>> qgrep . file ".py" and line "1"
./qgrep.py:1 #!/usr/bin/env python3
./qloc.py:1 #!/usr/bin/env python3
```
