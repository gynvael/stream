#!/usr/bin/python
import os, sys
from struct import pack, unpack

PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PATH)
import settings 

with open(PATH + "/base.bmp", "rb") as f:
  d = bytearray(f.read())

off = 0x436
out = d[:off]
raw = bytearray()

secret = iter(settings.MSG.ljust(0x10000, '\0'))

while off < len(d):
  opcode_or_len = d[off]
  out.append(d[off])
  off += 1

  if opcode_or_len != 0:
    raw.extend([d[off]] * opcode_or_len)
    out.append(d[off])    
    off += 1
    continue

  ext_opcode = d[off]
  out.append(d[off])
  off += 1

  if ext_opcode == 0:
    diff = 320 - len(raw) % 320
    if diff == 320:
      diff = 0
    raw.extend([0] * diff)
    #print("EOLN %u" % diff)
    continue  # eoln

  if ext_opcode == 1:
    #print("EOBMP")
    break  # eobmp

  if ext_opcode == 2:
    #print("XYZ")
    out.extend(d[off:off+2])
    off += 2
    continue  # xy

  #print("raw: %u" % ext_opcode)
  raw.extend(d[off:off+ext_opcode])
  out.extend(d[off:off+ext_opcode])
  off += ext_opcode
  if ext_opcode & 1:
    #print("RIFLE %.2x %.2x" % (ext_opcode, d[off]))
    #out.append(d[off])    
    out.append(next(secret))
    off += 1

#print("OFF=%x, LEN=%x" % (off, len(d)))

with open(PATH + "/level5.bmp", "wb") as f:
  f.write(out)

with open(PATH + "/level5.raw", "wb") as f:
  f.write(raw)

