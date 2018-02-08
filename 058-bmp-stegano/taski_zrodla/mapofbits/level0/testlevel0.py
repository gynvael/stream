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


with open(PATH + "/level0.bmp", "rb") as f:
  d = bytearray(f.read())

data_offset = rd(d, 0x0A)  # bfOffBits

i = data_offset
msg_bit = 0
byte = 0
msg = ""
while i < len(d):
  byte |= (d[i] & 1) << msg_bit

  msg_bit += 1
  if msg_bit == 8:
    if byte != 0:
      msg += chr(byte)
    byte = 0
    msg_bit = 0    

  i += 1

if str(settings.MSG) in msg:
  print "Level 0: OK"
  sys.exit(0)

print "Level 0: FAIL"
sys.exit(1)

