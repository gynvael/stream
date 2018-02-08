import sys
d = bytearray(open("level1.bmp", "rb").read())

px = d[0x36:]

o = []

for p in px:
  o.append(str(p&1))

o = ''.join(o)

print o

sys.exit("asdf")


i = 0

while i < len(o):
  b = int(o[i:i+8][::-1], 2)
  if b == 0:
    break

  sys.stdout.write(chr(b))

  i += 8

print ""




