import struct
# 0F 00 00 00 --> 0x000F
# F0 00 00 00 --> 0x00F0
# 00 0F 00 00 --> 0x0F00
#                   ↑

# a
# 7

# a7 7a

d = open("level4.bmp", "rb").read()[66:]

o = ""

i = -2
while True:
  i += 2
  if i >= len(d):
    break
  v = struct.unpack("<H", d[i:i+2])[0] >> 12
  o += f"{v:x}"

#print(b''.fromhex(o))
print(b''.fromhex(o[::-1])[::-1])






