d = open("level3.bmp", "rb").read()[40+14:]

b = b''
for i in range(207):
  b += bytes([ d[(313*3+1)*i + 313*3] ])

print(b)


