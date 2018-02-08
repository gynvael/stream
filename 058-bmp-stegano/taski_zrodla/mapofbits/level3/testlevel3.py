#!/usr/bin/python
import os, sys
from struct import pack, unpack

PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PATH)
import settings

def rd(d, off):
  return unpack("<I", str(d[off:off+4]))[0]


with open(PATH + "/level3.bmp", "rb") as f:
  d = bytearray(f.read())

data_offset = rd(d, 0x0A)  # bfOffBits
width = rd(d, 0x12)  # biWidth
height = rd(d, 0x16)  # biHeight

scanline_padding_offset = width * 3
scanline_size = (scanline_padding_offset + 3) & (~3)

msg = str(d[data_offset + scanline_padding_offset::scanline_size])
if settings.MSG in msg:
  print "Level 3: OK"
  sys.exit(0)

print "Level 3: FAIL"
sys.exit(1)




