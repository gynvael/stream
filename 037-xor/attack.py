from collections import *

key_length = 7

d = open("eng.txt", "rb").read().decode("utf-8")
od = []
for ch in d:
  if ord(ch) > 0x7e:
    continue
  od.append(chr(ord(ch)))

d = ''.join(od)

h = Counter(d)
print h.most_common(1)
most_common = ord(h.most_common(1)[0][0])

d = open("data.xorred", "rb").read()

key = ""
hin = []
for i in range(key_length):
  c = Counter(d[i::key_length])
  hin.append(c)
  print c.most_common(1)
  in_most_common = ord(c.most_common(1)[0][0])
  key += chr(most_common ^ in_most_common)

print key.encode("hex")

def xor(data, key):
  key = bytearray(key)
  data = bytearray(data)

  o = []
  for i, d in enumerate(data):
    o.append(chr(d ^ key[i % len(key)]))
    
  return ''.join(o)

print xor(d, key)














