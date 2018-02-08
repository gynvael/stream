#!/usr/bin/python
import os, sys
from struct import pack, unpack

PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PATH)
import settings

def rb(d, off):
  return d[off]

def rw(d, off):
  return unpack("<H", str(d[off:off+2]))[0]

def rd(d, off):
  return unpack("<I", str(d[off:off+4]))[0]


with open(PATH + "/base.bmp", "rb") as f:
  d = bytearray(f.read())

data_offset = rd(d, 0x0A)  # bfOffBits
width = rd(d, 0x12)  # biWidth
height = rd(d, 0x16)  # biHeight

scanline_padding_offset = width * 3
scanline_size = (scanline_padding_offset + 3) & (~3)

msg = bytearray(settings.MSG.ljust(height, '\0'))
d[data_offset + scanline_padding_offset::scanline_size] = msg

with open(PATH + "/level3.bmp", "wb") as f:
  f.write(d)


