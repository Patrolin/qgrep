from sys import argv
from os import walk
from collections import Counter

if __name__ == "__main__":
  loc_counters = Counter()
  rootPath = argv[1] if len(argv) > 1 else "."
  for (root, dirs, files) in walk(rootPath, topdown=True):
      root = root.replace("\\", "/")
      if root == "./.git" or root.startswith("./.git/"): continue
      for file in files:
        path = f"{root}/{file}"
        extension = path.split('.')[-1]
        acc_file_lines = 0
        with open(path, "r", encoding="utf8") as f:
            try:
              line = f.readline()
              while line:
                if line.strip():
                  acc_file_lines += 1
                line = f.readline()
            except (UnicodeDecodeError, PermissionError, OSError): # wtf
                pass
        if acc_file_lines:
          loc_counters[extension] += acc_file_lines
  for k, v in loc_counters.items():
    print(f".{k}: {v}")
