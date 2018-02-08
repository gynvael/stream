#!/usr/bin/python
import os, sys
from struct import pack, unpack

PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PATH)
import settings

with open(PATH + "/level5.bmp", "rb") as f:
  d = bytearray(f.read())

off = 0x436  # Data start.

flag = ""

while off < len(d):
  opcode_or_len = d[off]
  off += 1

  if opcode_or_len != 0:  # Encoded mode.
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
    off += 2
    continue  # XY Offset.

  # Absolute mode.
  off += ext_opcode
  if ext_opcode & 1:  # Padding?
    flag += chr(d[off])
    off += 1

if settings.MSG in flag:
  print "Level 5: OK"
  sys.exit(0)

print "Level 5: FAIL"
sys.exit(1) 

