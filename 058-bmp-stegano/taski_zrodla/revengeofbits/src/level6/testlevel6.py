#!/usr/bin/python
import os, sys
from struct import pack, unpack

PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PATH)
import settings

with open(PATH + "/level6.bmp", "rb") as f:
  d = bytearray(f.read())

off = 0x436  # Data start.

flag = ""
byte = 0
bit = 0
data = False
marker_cnt = 0

while off < len(d):
  opcode_or_len = d[off]
  off += 1

  if opcode_or_len != 0:  # Encoded mode.

    if marker_cnt == 3:
      #print "START", off
      data = True
      marker_cnt = 0

    if marker_cnt == 2:
      #print "STOP ", off
      data = False
      marker_cnt = 0

    # Special case:
    if marker_cnt == 2 + 3:
      #print "RESTART ", off
      data = True
      marker_cnt = 0

    if data:
      #print opcode_or_len
      byte |= (opcode_or_len & 1) << bit
      bit += 1
      if bit == 8:
        flag += chr(byte)
        byte = 0
        bit = 0

    off += 1
    continue

  # Escape.
  ext_opcode = d[off]
  off += 1

  if ext_opcode == 0:
    continue  # EoLn.

  if ext_opcode == 1:
    break  # EoBMP.

  if ext_opcode == 2:
    if d[off] == 0 and d[off+1] == 0:
      marker_cnt += 1
    off += 2
    continue  # XY Offset.

  # Absolute mode.
  off += ext_opcode
  if ext_opcode & 1:
    off += 1

#print flag

if settings.MSG in flag:
  print "Level 6: OK"
  sys.exit(0)

print "Level 6: FAIL"
sys.exit(1) 

