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

with open(PATH + "/base.bmp", "rb") as f:
  d = bytearray(f.read())

data_offset = rd(d, 0x0A)  # bfOffBits
width = rd(d, 0x12)  # biWidth
height = rd(d, 0x16)  # biHeight

header = ''.join([
    # File header.
    "BM",
    dd(14 + 40 + 3 * 4 + width * height * 2),
    dw(0), dw(0),
    dd(14 + 40 + 3 * 4),

    # Info header.
    dd(40),
    dd(width), dd(height),
    dw(1),
    dw(16),  # bpp
    dd(3),  # BI_BITFIELDS
    dd(width * height * 2),
    dd(0) * 4,

    # Masks.
    dd(0x0000000F),
    dd(0x000000F0),
    dd(0x00000F00)
])

bitmap = []
i = data_offset
msg_bit = 0
msg_byte = 0

for _ in range(width * height):
  m_nibble = 0
  if msg_byte < len(settings.MSG):
    m_nibble = (ord(settings.MSG[msg_byte]) >> msg_bit) & 0xF

  r, g, b = d[i:i+3]
  bitmap.append(dw(
    (r >> 4) |
    ((g >> 4) << 4) |
    ((b >> 4) << 8) |
    (m_nibble << 12)
  ))

  i += 3
  msg_bit += 4
  if msg_bit == 8:
    msg_byte += 1
    msg_bit = 0

with open(PATH + "/level4.bmp", "wb") as f:
  f.write(header)
  f.write(''.join(bitmap))


