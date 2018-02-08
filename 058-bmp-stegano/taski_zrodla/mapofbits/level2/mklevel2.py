#!/usr/bin/python
import os, sys
from struct import pack, unpack

PATH = os.path.dirname(os.path.realpath(__file__))

def rb(d, off):
  return d[off]

def rw(d, off):
  return unpack("<H", str(d[off:off+2]))[0]

def rd(d, off):
  return unpack("<I", str(d[off:off+4]))[0]

def dd(d, off, v):
  d[off:off+4] = bytearray(pack("<I", v))

with open(PATH + "/base_a.bmp", "rb") as f:
  d_a = bytearray(f.read())

with open(PATH + "/base_b.bmp", "rb") as f:
  d_b = bytearray(f.read())

data_offset = rd(d_a, 0x0A)  # bfOffBits

bmp_a = d_a[data_offset:]
bmp_b = d_b[data_offset:]
header = d_a[:data_offset]
dd(header, 0x0A, data_offset + len(bmp_b))

with open(PATH + "/level2.bmp", "wb") as f:
  f.write(header)
  f.write(bmp_b)
  f.write(bmp_a)  

