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
```

Case insensitive by default
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep
>> qgrep: . "ářgůmënt" and file "qgrep.py"
./qgrep.py:3 from argparse import ArgumentParser, BooleanOptionalAction
./qgrep.py:224         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py:225         argument_parser.add_argument('-d', action=BooleanOptionalAction, help='print verbose information')
./qgrep.py:226         argument_parser.add_argument('-c', action=BooleanOptionalAction, help='use case sensitive comparisons ("A" != "a")')
./qgrep.py:227         argument_parser.add_argument('-a', action=BooleanOptionalAction, help='use accent sensitive comparisons ("á" != "a")')
./qgrep.py:228         argument_parser.add_argument('-s', action=BooleanOptionalAction, help='use symbol sensitive comparisons ("ﬁ" != "fi")')
./qgrep.py:230         arguments = argument_parser.parse_args(argv[1:])
./qgrep.py:231         is_debug = arguments.d
./qgrep.py:232         is_case_sensitive = arguments.c
./qgrep.py:233         is_accent_sensitive = arguments.a
./qgrep.py:234         is_symbol_sensitive = arguments.s
./qgrep.py:236         argumentsString = " " + "".join(argv[1:]) if len(argv) > 1 else ""
./qgrep.py:238             match = re.search(r"(\S+)\s(.*)", input(f">> qgrep{argumentsString}: ").strip())
```

Case sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -c
>> qgrep -c: . "Argument" and file "grep.py"
./qgrep.py:3 from argparse import ArgumentParser, BooleanOptionalAction
./qgrep.py:224         argument_parser = ArgumentParser(description='Search files for strings.')
```

Accent sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -a
>> qgrep -a: . "Á" and file "grep.py"
./qgrep.py:227         argument_parser.add_argument('-a', action=BooleanOptionalAction, help='use accent sensitive comparisons ("á" != "a")')
```

Symbol sensitive flag
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -s
>> qgrep -s: . "ﬁ" and file "grep.py"
./qgrep.py:228         argument_parser.add_argument('-s', action=BooleanOptionalAction, help='use symbol sensitive comparisons ("ﬁ" != "fi")')
```


All binary operators are left-associative with no priority
```py
PS C:\Users\Patrolin\Documents\qGrep> qgrep -d
>> qgrep -d: . "with " or "for " and file "qgrep.py"
debug: (("with " or "for ") and (file "qgrep.py"))
./qgrep.py:197     acc = acc if is_accent_sensitive else "".join(v for v in acc if not unicodedata.combining(v))
./qgrep.py:224         argument_parser = ArgumentParser(description='Search files for strings.')
./qgrep.py:249             for dir_path_match in glob(dir_path):
./qgrep.py:251                 for (root, dirs, files) in walk(dir_path_match, topdown=True):
./qgrep.py:254                     for file in files:
./qgrep.py:257                             with open(path, "r", encoding="utf8") as f:
./qgrep.py:261                                 for i, line in enumerate(f.readlines()):
```

Bracket support
```py
>> qgrep -d: . "for" and not "walk" or ("while" and "try") and file "www"
debug: ((("for" and (not "walk")) or ("while" and "try")) and (file "www"))
```
