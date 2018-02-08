#!/usr/bin/python
import os, sys
from struct import pack, unpack

PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PATH)
import settings 

def dd(d, off, v):
  d[off:off+4] = bytearray(pack("<I", v))

with open(PATH + "/base.bmp", "rb") as f:
  d = bytearray(f.read())

off = 0x436
out = d[:off]
raw = bytearray()

secret = iter(bytearray(settings.MSG))
byte = next(secret)
s_bit = 0

while off < len(d):
  opcode_or_len = d[off]

  if opcode_or_len != 0:
  
    # Too little space to bother?
    if opcode_or_len < 10 or byte is None:
      out.append(d[off])
      off += 1
      raw.extend([d[off]] * opcode_or_len)      
      out.append(d[off])
      off += 1
      continue

    # Grab argument.
    off += 1
    color = d[off]
    off += 1

    out.extend([0, 2, 0, 0] * 3) # Start sequence.
    #print len(out)

    # Until there are less than 3 left...
    K = 2
    while opcode_or_len >= (K + 1):
      # Grab a bit.
      bit = (byte >> s_bit) & 1

      # Encode it.
      length = K + bit
      #print length
      raw.extend([color] * length)
      out.extend([length, color])
      #print length, color

      # Iterate.
      opcode_or_len -= length
      s_bit += 1
      if s_bit == 8:
        try:
          byte = next(secret)
        except StopIteration:
          byte = None
          break
        s_bit = 0
      
    out.extend([0, 2, 0, 0] * 2) # End sequence.

    # Store the rest, if any.
    if opcode_or_len:
      #print opcode_or_len
      raw.extend([color] * opcode_or_len)
      out.extend([opcode_or_len, color])

    continue

  out.append(d[off])
  off += 1

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
    out.append(0)
    #out.append(next(secret))
    off += 1

#print("OFF=%x, LEN=%x" % (off, len(d)))

# Fix up.
dd(out, 2, len(out))
dd(out, 0x22, len(out) - 0x436)

with open(PATH + "/level6.bmp", "wb") as f:
  f.write(out)

with open(PATH + "/level6.raw", "wb") as f:
  f.write(raw)

