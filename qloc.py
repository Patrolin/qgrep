from sys import argv
from os import walk
from collections import Counter

if __name__ == "__main__":
  loc_counter_groups = [
    Counter(),
    Counter(),
    Counter(),
  ]
  rootPath = argv[1] if len(argv) > 1 else "."
  for (root, dirs, files) in walk(rootPath, topdown=True):
      root = root.replace("\\", "/")
      if root == "./.git" or root.startswith("./.git/"): continue
      for file in files:
        file_path = f"{root}/{file}"
        pre_file_path, extension = file_path.rsplit('.', maxsplit=1)
        file_name = pre_file_path.rsplit('/', maxsplit=1)[-1]
        acc_file_lines = 0
        with open(file_path, "r", encoding="utf8") as f:
            try:
              line = f.readline()
              while line:
                if line.strip():
                  acc_file_lines += 1
                line = f.readline()
            except (UnicodeDecodeError, PermissionError, OSError): # wtf
                pass
        if acc_file_lines:
          if file_name: # qgrep.py
            loc_counter_groups[2][extension] += acc_file_lines
          elif extension.startswith('/'): # /LICENSE
            loc_counter_groups[1][extension] += acc_file_lines
          else: # .gitignore
            loc_counter_groups[0][extension] += acc_file_lines
  # print
  for group in loc_counter_groups:
    loc_keys = sorted(group)
    for k in loc_keys:
      print(f".{k}: {group[k]}")
