d = open("task.bmp", "rb").read()

di = 0x234 - 0x100
si = 0x1dc - 0x100
bx = 0x208 - 0x100

sz = 0x2c

o = ""
for i in range(sz):
  o += chr(d[di + i] ^ d[si + i] ^ d[bx + sz - 1 - i])
  print(o)




