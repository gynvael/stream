import sys
lines = open("level6.dec").read().splitlines()

decode = False
idx = 0
acc = 0
flag = ""

for i, ln in enumerate(lines):
  ln = ln.strip()

  if not decode:
    if ln == "DECODE_START":
      decode = True
    continue

  if ln == "DECODE_END":
    decode = False
    continue

  if ln == "DECODE_START":
    sys.exit(f"??? {i}")

  try:
    x = int(ln.split(" ")[1][:-1], 16) & 1
  except IndexError:
    print(ln)
  acc |= x << idx
  idx += 1

  if idx == 8:
    flag += chr(acc)
    acc = 0
    idx = 0
    print(len(flag), i)

print(flag)



