import sys
d = bytearray(open("level3.bmp", "rb").read())

px = d[0x36:]

o = ""
for i in range(207):
  idx = (313 * 3 + 1) * i
  o += chr(px[idx + 313 * 3])

print o

