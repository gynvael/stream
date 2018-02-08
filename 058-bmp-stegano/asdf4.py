import sys
d = bytearray(open("level4.bmp", "rb").read())

px = d[66:]

o = ""

for i in range(212):
  idx = 320 * 2 * i
  for j in range(320):
    p = (px[idx + j * 2 + 1] & 0xf0) >> 4
    o += "%x" % p

print o[::-1].decode("hex")[::-1]



