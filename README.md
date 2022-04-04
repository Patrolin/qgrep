# qGrep
a windows version of grep <s>in zig-0.10.0-dev.1699+8b5d5f44e</s> in Python because Zig can't tell a file from a folder

### usage
```ps
PS C:\Users\Patrolin\Documents\qGrep> qgrep
qgrep: . "print("
"print("
./qgrep.py:     print(root)
./qgrep.py:                                 print(f"{path}: {line[:-1]}")
./qgrep.zig:         try stdout.print("{s}: {s}\n", .{walkerEntry.path, walkerEntry.kind});
./qgrep.zig:             try stdout.print("{s}\n", .{err});
./qgrep.zig:                 try stdout.print("{s}: {s}\n", .{path, line});
qgrep: . "print(" and not "try"
("print(" and (not "try"))
./qgrep.py:     print(root)
./qgrep.py:                                 print(f"{path}: {line[:-1]}"
```
