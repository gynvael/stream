def load_file(fname):
  with open(fname, "rb") as f:
    return bytearray(f.read())

data = [
  (5, load_file("men_05.vsf")),
  (4, load_file("men_04.vsf")),
  (3, load_file("men_03.vsf")),
  (2, load_file("men_02.vsf")),
]

for i in xrange(len(data[0][1])):
  found = True
  for v, d in data:
    if d[i] != v:
      found = False
      break

  if not found:
    continue

  print "Found: 0x%x" % i


