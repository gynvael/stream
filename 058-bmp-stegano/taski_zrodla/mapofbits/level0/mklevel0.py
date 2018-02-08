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

i = data_offset
msg_byte = 0
msg_bit = 0
while i < len(d):
  # Fetch.
  data_bit = 0

  if msg_byte < len(settings.MSG):
    data_bit = (settings.MSG[msg_byte] >> msg_bit) & 1

  # Insert.
  d[i] = (d[i] & 0xFE) | data_bit

  # Iter.
  msg_bit += 1
  if msg_bit == 8:
    msg_byte += 1
    msg_bit = 0
  i += 1

with open(PATH + "/level0.bmp", "wb") as f:
  f.write(d)


