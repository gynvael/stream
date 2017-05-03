key =  "30daa3363d4571".decode("hex")
text = "czesc co tam porabiacie"

def xor(data, key):
  key = bytearray(key)
  data = bytearray(data)

  o = []
  for i, d in enumerate(data):
    o.append(chr(d ^ key[i % len(key)]))
    
  return ''.join(o)

d = open("data.txt").read()
e = xor(d, key)

open("data.xorred", "wb").write(e)







