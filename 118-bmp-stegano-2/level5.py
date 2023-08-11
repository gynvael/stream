"""
AAABBBBBCCCAAAABBB

3A 5B 3C 4A 3B       1
2A 1A 5B 3C 4A 3B    0
1A 2A 5B 3C 4A 3B

AAABBBBBCCCAAAABBB

0 8 QWERTYUI
0 7 QWERTYU\0

0 0 end of line
0 1 end of bitmap
0 2 x y

1Q 1W 1E 1R 1T 1Y 1U 1I
"""
offset = 0x436

d = open("level5.bmp", "rb").read()[offset:]

#d = b"\x04\x16\x08\x45\0\0\0\2\4\2\3\xE4\0\3\x12\xa4\x46\0\0\0\0\1"

padding = ""

i = 0
while i < len(d):

  if d[i] == 0:
    i += 1
    count = d[i]
    i += 1

    if count == 0:
      print("end of scanline")
      continue

    if count == 1:
      print("END OF BITMAP")
      continue

    if count == 2:
      x, y = d[i], d[i+1]
      i += 2
      print(f"XY: {x}, {y}")
      continue

    raw = d[i:i+count]
    i += count

    if count % 2:
      p = d[i]
      padd = f"{p:02x}"
      i += 1
      padding += chr(p)
    else:
      padd = "xx"

    print(f"RAW: [{padd}] {count:02x} --> {raw}")
    continue

  count = d[i]
  index = d[i+1]
  i += 2
  print(f"RLE: {count:02x}, {index:02x}")

print(padding)




