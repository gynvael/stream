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

def db(v):
  return pack("<B", v)

def dw(v):
  return pack("<H", v)

def dd(v):
  return pack("<I", v)

def dq(v):
  return pack("<Q", v)

with open(PATH + "/level4.bmp", "rb") as f:
  d = bytearray(f.read())

data_offset = rd(d, 0x0A)  # bfOffBits
width = rd(d, 0x12)  # biWidth
height = rd(d, 0x16)  # biHeight

msg = ""
byte = 0
msg_bit = 0
i = data_offset
for _ in range(width * height):
  byte |= (rw(d, i) >> 12) << msg_bit 

  i += 2
  msg_bit += 4
  if msg_bit == 8:
    msg += chr(byte)
    msg_bit = 0
    byte = 0

if settings.MSG in msg:
  print "Level 4: OK"
  sys.exit(0)

print "Level 4: FAIL"
sys.exit(1)


