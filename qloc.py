from sys import argv
from os import walk
from collections import Counter

if __name__ == "__main__":
  extension_to_loc = Counter()
  rootPath = argv[1] if len(argv) > 1 else "."
  for (root, dirs, files) in walk(rootPath, topdown=True):
      root = root.replace("\\", "/")
      if root == "./.git" or root.startswith("./.git/"): continue
      for file in files:
        path = f"{root}/{file}"
        extension = path.split('.')[-1]
        with open(path, "r", encoding="utf8") as f:
            line = f.readline()
            while line:
              if line.strip():
                extension_to_loc[extension] += 1
              line = f.readline()
  for k, v in extension_to_loc.items():
    print(f".{k}: {v}")
