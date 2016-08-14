import collections
KEY_LENGTH = 7

d = open("enc.bin", "rb").read()

KEY = []

for i in range(KEY_LENGTH):
  hist = {}
  data_set = d[i::KEY_LENGTH]
  c = collections.Counter(data_set)
  KEY.append(chr(ord(c.most_common()[0][0]) ^ 0x20))

m = []
for i, c in enumerate(d):
  m.append(chr(ord(c) ^ ord(KEY[i % len(KEY)])))

print ''.join(m)






