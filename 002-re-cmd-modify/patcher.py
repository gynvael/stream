import struct
import os

def dd(v):
  return map(ord, list(struct.pack("<I", v)))

def patch(d, offset, what):
  for i, byte in enumerate(what):
    d[offset + i] = byte

def compile_and_load(name):
  os.system("nasm \"%s.asm\"" % name)
  with open(name, "rb") as f:
    return map(ord, list(f.read()))

with open("asdf.exe", 'rb') as f:
  d = map(ord, list(f.read()))

print "-- Fixing file header"
d[0x106] |= 1

print "-- Fixing sections"
characteristics = 0x60000020  # code + RX
patch(d, 0x30c, dd(characteristics))

print "-- Put trash in .reloc"
patch(d, 0x39000, [0xcc] * 0x12c)

print "-- Patch 1"
patch(d, 0xad3e, compile_and_load("patch1"))
patch(d, 0x39000, compile_and_load("reloc1"))





with open("new.exe", 'wb') as f:
  f.write(''.join(map(chr, d)))





